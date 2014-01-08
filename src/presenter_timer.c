#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static int iInterval=5; //default
static int nInterval=10;
static int Intervals[]={5,15,20,25,30,45,60,75,90,120};
static char str[]="999:99 min (max)";

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Starting timer");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (iInterval<nInterval-1)
  {
    iInterval++;
    snprintf(str,8,"%3d min",Intervals[iInterval]);
  } 
  else
    snprintf(str,14,"%3d min (max)",Intervals[nInterval-1]);
  text_layer_set_text(text_layer, str);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (iInterval>0)
  {
    iInterval--;
    snprintf(str,8,"%3d min",Intervals[iInterval]);
  } 
  else
    snprintf(str,14,"%3d min (min)",Intervals[0]);
  text_layer_set_text(text_layer, str);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
  app_event_loop();
  deinit();
}
