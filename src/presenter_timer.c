#include <pebble.h>

// TODO: graphics, storing settings in the phone..

typedef struct mytimer_struct {
    int min;
    int sec;
} mytimer;
static Window *window;
static TextLayer *text_layer1;
static TextLayer *text_layer2;
static int iInterval=5; //default
static int Intervals[]={3,5,15,20,25,30,45,60,75,90,120,180};
#define nInterval 12
#define PrestartTime 5
static char str[]="999:99 min (max)";
static int started=0; //0=initial stage 1=prestart 2=timer running 3=finished
static mytimer TimerData;
static AppTimer *one_second;
static const VibePattern prestart_beep = {
  .durations = (uint32_t []) {50},
  .num_segments = 1
};
static const VibePattern start_main = {
  .durations = (uint32_t []) {250},
  .num_segments = 1
};


static void display_interval() {
  //shows current interval during initial setting
  snprintf(str,9,"%d\nmin",Intervals[iInterval]);
  // if (iInterval<nInterval-1 && iInterval>0)
  //   snprintf(str,9,"%d\nmin",Intervals[iInterval]);
  // else
  // {
  //   if (iInterval<nInterval-1)
  //     snprintf(str,16,"%d\nmin\n(min)",Intervals[0]);
  //   else
  //     snprintf(str,16,"%d\nmin\n(max)",Intervals[nInterval-1]);
  // }
  text_layer_set_text(text_layer1, str);
}

static void display_timer() {
  //shows current value of the timer
  snprintf(str,6,"%d:%02d",TimerData.min,TimerData.sec);
  text_layer_set_text(text_layer2, str); 
  layer_mark_dirty(text_layer_get_layer(text_layer2));
}

static void timeup() {
  //exit when timer is at 00:00
  //vibrate a lot and exit
  started=3;//mark that time finished
  tick_timer_service_unsubscribe();
  //hide textlayer 2 and show textlayer 1
  layer_set_hidden(text_layer_get_layer(text_layer2),true);
  layer_set_hidden(text_layer_get_layer(text_layer1),false);
  text_layer_set_text(text_layer1, "Time's up");
  vibes_long_pulse();
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  // APP_LOG(APP_LOG_VERBOSE, "Time flies!");
  if (started==1) {
    if (TimerData.sec<0) TimerData.sec=PrestartTime; //first tick --> start prestart
    else
    {
      //prestart
      if (TimerData.sec>1) TimerData.sec--; //continue prestart
      else
      { //prestart is done
        started=2;
        //initialize main timer
        TimerData.min=Intervals[iInterval];
        TimerData.sec=0;
        vibes_enqueue_custom_pattern(start_main);
      }
    }
  }
  else
  { //timer running: decrease by 1 sec
    TimerData.sec--;
    if (TimerData.sec==0 && TimerData.min==0) 
    {
      timeup(); //stop and exit
      return;
    }
    if (TimerData.sec<0)
    {
      TimerData.sec=59;
      TimerData.min--;
    } 
  }
  //chage display
  display_timer();
  //vibrate
  if (started==1 && TimerData.sec<=5) vibes_enqueue_custom_pattern(prestart_beep);
  if (TimerData.min==5 && TimerData.sec==0 && Intervals[iInterval]>5) vibes_double_pulse(); //on 5 min
  if (TimerData.min==1 && TimerData.sec==0 && Intervals[iInterval]>1) vibes_double_pulse(); //on 1 min
  //on half time
  if (Intervals[iInterval]%2==0)
  { //even number of minutes
      if (TimerData.min==Intervals[iInterval]/2 && TimerData.sec==0) vibes_double_pulse(); //on halftime
  }
  else
  { //off number of minutes
      if (TimerData.min==Intervals[iInterval]/2 && TimerData.sec==30) vibes_double_pulse(); //on halftime
  }
}

static void run_timer() {
  //all the action of running the timer
  started=1;//disable buttons
  //hide textlayer 1 and show textlayer 2
  layer_set_hidden(text_layer_get_layer(text_layer1),true);
  layer_set_hidden(text_layer_get_layer(text_layer2),false);
  //initialize timer
  TimerData.min=0;
  TimerData.sec=-1;
  //start timer
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  //Show ready message (to wait for the first tick)
  //text_layer_set_text(text_layer2, "Ready!");
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (started) return; //don't do anything if started
  run_timer();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (started) return; //don't do anything if started
  if (iInterval<nInterval-1) iInterval++;//increment up to max
  display_interval();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (started) return; //don't do anything if started
  if (iInterval>0) iInterval--;//decrememnt down to min
  display_interval();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  //main window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(window, GColorBlack); //background

  //text layer for initial setup phase
  text_layer1 = text_layer_create((GRect) { .origin = { 0, 25 }, .size = { bounds.size.w, bounds.size.h-25 } }); //location!
  text_layer_set_text_color(text_layer1, GColorWhite);
  text_layer_set_background_color(text_layer1, GColorClear);
  text_layer_set_text_alignment(text_layer1, GTextAlignmentCenter);
  text_layer_set_font(text_layer1, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));

  //text layer for running timer
  text_layer2 = text_layer_create((GRect) { .origin = { 0, 100 }, .size = { bounds.size.w, 68 } }); // 144x168, 16 top pixels occupied
  text_layer_set_text_color(text_layer2, GColorWhite);
  text_layer_set_background_color(text_layer2, GColorClear);
  text_layer_set_text_alignment(text_layer2, GTextAlignmentCenter);
  text_layer_set_font(text_layer2, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));


  //add text layer as a child to window
  layer_add_child(window_layer, text_layer_get_layer(text_layer1));
  layer_add_child(window_layer, text_layer_get_layer(text_layer2));

  //display default timer interval
  display_interval();
  //hide layer2
  layer_set_hidden(text_layer_get_layer(text_layer2),true);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer1);
  text_layer_destroy(text_layer2);
  if (started && started!=3) tick_timer_service_unsubscribe(); //only unsubscribe here if time didn't run out
}

int main(void) {
  //init
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  //log
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
  //main loop
  app_event_loop();
  //clean up (all children are destroyed in .unload)
  window_destroy(window);
}
