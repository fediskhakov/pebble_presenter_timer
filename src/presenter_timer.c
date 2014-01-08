#include <pebble.h>

typedef struct mytimer_struct {
    int min;
    int sec;
} mytimer;

static Window *window;
static TextLayer *text_layer;
static int iInterval=5; //default
static int nInterval=10;
static int Intervals[]={5,15,20,25,30,45,60,75,90,120};
static char str[]="999:99 min (max)";
static int started=0;
static mytimer timer;

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
  text_layer_set_text(text_layer, str);
}

static void run_timer() {
  //all the action of running the timer
  started=1;
  text_layer_set_text(text_layer, "Timer started!");
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

  //text layer
  text_layer = text_layer_create((GRect) { .origin = { 0, 25 }, .size = { bounds.size.w, bounds.size.h-25 } }); //location!
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));

  //add text layer as a child to window
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  //display default timer interval
  display_interval();
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
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
