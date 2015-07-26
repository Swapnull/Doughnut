#include "pebble.h"
	
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
	
	
static Window *window;
static Layer *s_background_layer, *s_display_layer;
static TextLayer *s_time_layer;
static BitmapLayer *s_weather_layer;
static GBitmap *s_weather_bitmap;

static void time_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
	
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
	
	
	graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
	
	
	//SECOND 
	//circle
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_draw_circle(ctx, center, 70);
	//graphics_fill_circle(ctx, center, 70);
  
	graphics_context_set_stroke_width(ctx, 6);

	//hand
	int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  GPoint second_hand = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)67 / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)67 / TRIG_MAX_RATIO) + center.y,
  };
  graphics_context_set_stroke_color(ctx, GColorOrange);
  graphics_draw_line(ctx, second_hand, center);
	
	
	//MINUTE
	
	//circle
	graphics_context_set_fill_color(ctx, GColorOrange);
	graphics_fill_circle(ctx, center, 60);
	
	//hand
	int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  GPoint minute_hand = {
    .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)57 / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)57 / TRIG_MAX_RATIO) + center.y,
  };
  graphics_context_set_stroke_color(ctx, GColorBlue);
  graphics_draw_line(ctx, minute_hand, center);
	
	//HOUR
	
	//circle
	graphics_context_set_fill_color(ctx, GColorBlue);
	graphics_fill_circle(ctx, center, 50);	
	
	//hand
	int32_t hour_angle = TRIG_MAX_ANGLE * t->tm_hour / 12;
  GPoint hour_hand = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)47 / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)47 / TRIG_MAX_RATIO) + center.y,
  };
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, hour_hand, center);
	
	
	//Center Details
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, center, 40);
	
  // Create a long-lived buffer
  static char buffer[] = "00-00";
  strftime(buffer, sizeof("00-00"), "%d/%m", t);
	text_layer_set_text(s_time_layer, buffer);
	
	
	if(t->tm_min % 15 == 0 && t->tm_sec == 1){
		APP_LOG(APP_LOG_LEVEL_INFO, "time is %i:%i", t->tm_hour ,t->tm_min);
		DictionaryIterator *iterator;
		app_message_outbox_begin(&iterator);
		dict_write_int8(iterator, 0, 0);
		
		app_message_outbox_send();
	}
}
	
static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
	
	s_background_layer = layer_create(bounds);
	layer_add_child(window_layer, s_background_layer);
	
  s_display_layer = layer_create(bounds);
  layer_set_update_proc(s_display_layer, time_update_proc);
  layer_add_child(window_layer, s_display_layer);
		
	s_time_layer = text_layer_create(GRect(57, 60, 35, 20));
	text_layer_set_background_color(s_time_layer, GColorBlack);
	text_layer_set_text_color(s_time_layer, GColorWhite);
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
		
		
	s_weather_layer = bitmap_layer_create(GRect(47, 80, 50, 35));
	layer_add_child(window_layer, bitmap_layer_get_layer(s_weather_layer));
}

static void window_unload(Window *window) {
  layer_destroy(s_background_layer);
  layer_destroy(s_display_layer);
	text_layer_destroy(s_time_layer);
  bitmap_layer_destroy(s_weather_layer);
	gbitmap_destroy(s_weather_bitmap);
}

static uint32_t getWeatherResource(char conditions[]){
	
	
	if(! strcmp(conditions, "01d")){
		return RESOURCE_ID_WEATHER_1_DAY;
	}else if(! strcmp(conditions,"01n")){
	  return RESOURCE_ID_WEATHER_1_NIGHT;
	}else if(! strcmp(conditions,"02d")){
	  return RESOURCE_ID_WEATHER_2_DAY;
	}else if(! strcmp(conditions,"02n")){
	  return RESOURCE_ID_WEATHER_2_NIGHT;
	}else if(! strcmp(conditions,"03d")){
	  return RESOURCE_ID_WEATHER_3_DAY;
	}else if(! strcmp(conditions,"03n")){
	  return RESOURCE_ID_WEATHER_3_DAY;
	}else if(! strcmp(conditions,"04d")){
	  return RESOURCE_ID_WEATHER_4_DAY;
	}else if(! strcmp(conditions,"04n")){
	  return RESOURCE_ID_WEATHER_4_NIGHT;
	}else if(! strcmp(conditions,"09d")){
	  return RESOURCE_ID_WEATHER_9_DAY;
	}else if(! strcmp(conditions,"09n")){
	  return RESOURCE_ID_WEATHER_9_NIGHT;
	}else if(! strcmp(conditions,"10d")){
	  return RESOURCE_ID_WEATHER_10_DAY;
	}else if(! strcmp(conditions,"10n")){
	  return RESOURCE_ID_WEATHER_10_NIGHT;
	}else if(! strcmp(conditions,"11d")){
	  return RESOURCE_ID_WEATHER_11_DAY;
	}else if(! strcmp(conditions,"11n")){
	  return RESOURCE_ID_WEATHER_11_NIGHT;
	}else if(! strcmp(conditions,"13d")){
	  return RESOURCE_ID_WEATHER_13_DAY;
	}else if(! strcmp(conditions,"13n")){
	  return RESOURCE_ID_WEATHER_13_NIGHT;
	}	else if(! strcmp(conditions,"50d")){
	  return RESOURCE_ID_WEATHER_50_DAY;
	} else if(! strcmp(conditions,"50n")){
	  return RESOURCE_ID_WEATHER_50_NIGHT;
	}else {
		APP_LOG(APP_LOG_LEVEL_ERROR, "Unknown weather resource id");
		return RESOURCE_ID_WEATHER_50_DAY;
	}
		
}

static void inbox_received_callback(DictionaryIterator *iterator, void *ctx){
	// Store incoming info
  static char temperature_buffer[8];
  static char conditions_buffer[32];
	
	// Read first item
	Tuple *t = dict_read_first(iterator);
	
	//For all items
	while(t != NULL){
		switch(t->key){
			case KEY_TEMPERATURE:
			  snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);
			  break;
			case KEY_CONDITIONS:
			  snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
			  break;
		  default:
			  APP_LOG(APP_LOG_LEVEL_ERROR, "key %d not recognised", (int)t->key);
			  break;
		}
		
		t = dict_read_next(iterator);
	}
	
	s_weather_bitmap = gbitmap_create_with_resource(getWeatherResource(conditions_buffer));
	bitmap_layer_set_bitmap(s_weather_layer,s_weather_bitmap);
}



static void inbox_dropped_callback(AppMessageResult reason, void *ctx){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message Dropped");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *ctx){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *ctx){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send success");
}


static void init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
	
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());	
	
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}