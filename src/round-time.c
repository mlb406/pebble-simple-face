#include <pebble.h>

static Window *window;
static Layer *hands_layer, *bg_layer;


#ifdef PBL_RECT
  int minute_length = 66;
  int hour_length = 36;
  int bg_markers_1 = 72;
  int bg_markers_2 = 69;
#else  
  int minute_length = 80;
  int hour_length = 46;
  int bg_markers_1 = 90;
  int bg_markers_2 = 86;
#endif

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void bg_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  int i;
  for (i = 0; i < 12; i++) {
    
    int32_t minute_angle = TRIG_MAX_ANGLE * i / 12;
    GPoint minute_hand_1 = {
      .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)bg_markers_2 / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)bg_markers_2 / TRIG_MAX_RATIO) + center.y,
    };
    
    GPoint minute_hand_2 = {
      .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)bg_markers_1 / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)bg_markers_1 / TRIG_MAX_RATIO) + center.y,
    };
    graphics_context_set_stroke_color(ctx, GColorWhite);
    
    #ifdef PBL_COLOR
      graphics_context_set_stroke_width(ctx, 2);
    #endif
    if (minute_angle == 0) {
      #ifdef PBL_ROUND
        graphics_draw_line(ctx, minute_hand_1, minute_hand_2);
      #else 
        graphics_draw_line(ctx, GPoint(72, 0), GPoint(72, 3));
      #endif
    } else if (i == 3) { 
      graphics_draw_line(ctx, minute_hand_1, minute_hand_2);
    } else if (i == 6) {
      #ifdef PBL_ROUND
        graphics_draw_line(ctx, minute_hand_1, minute_hand_2);
      #else 
        graphics_draw_line(ctx, GPoint(72, 168), GPoint(72, 165));
      #endif
    } else if (i == 9) {
      graphics_draw_line(ctx, minute_hand_1, minute_hand_2);
    }
    
  }
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  time_t temp = time(NULL);
  struct tm *t = localtime(&temp);
  
  //----------------------------------------------------------------------------------
  
  int32_t  second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  GPoint second_point = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)minute_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)minute_length / TRIG_MAX_RATIO) + center.y,
  };
  
  
    graphics_context_set_stroke_width(ctx, 2);
    #ifdef PBL_COLOR
    graphics_context_set_stroke_color(ctx, GColorFromRGB(255, 255, 0));
    #else 
    graphics_context_set_stroke_color(ctx, GColorWhite);
    #endif
  
  
  graphics_draw_line(ctx, center, second_point);
  
  //----------------------------------------------------------------------------------
  
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  GPoint minute_point = {
    .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)minute_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)minute_length / TRIG_MAX_RATIO) + center.y,
  };
  graphics_context_set_stroke_width(ctx, 6);
  #ifdef PBL_COLOR
  graphics_context_set_stroke_color(ctx, GColorFromRGB(0, 0, 255));
  #else 
  graphics_context_set_stroke_color(ctx, GColorLightGray);
  #endif
  
  graphics_draw_line(ctx, center, minute_point);
  
  //----------------------------------------------------------------------------------
  
  int32_t hour_angle = TRIG_MAX_ANGLE * t->tm_hour / 12;
  GPoint hour_hand = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hour_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hour_length / TRIG_MAX_RATIO) + center.y,
  };
  
  #ifdef PBL_COLOR
  graphics_context_set_stroke_color(ctx, GColorFromRGB(255, 0, 0));
  #else 
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  #endif
  
  graphics_draw_line(ctx, center, hour_hand);
  
  //----------------------------------------------------------------------------------
  
  graphics_context_set_fill_color(ctx, GColorWhite);
  
  graphics_fill_circle(ctx, center, 3);
  
}

static void main_window_load() {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  hands_layer = layer_create(bounds);
  layer_set_update_proc(hands_layer, hands_update_proc);
  layer_add_child(window_layer, hands_layer);
  
  bg_layer = layer_create(bounds);  
  layer_set_update_proc(bg_layer, bg_update_proc);
  layer_add_child(window_layer, bg_layer);
}

static void main_window_unload() {
  layer_destroy(bg_layer);
}

static void init() {
  window = window_create();
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  window_set_background_color(window, GColorBlack);
  
  window_stack_push(window, true);
}

static void deinit() {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
  return 0;
}