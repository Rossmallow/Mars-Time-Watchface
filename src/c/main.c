#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_mars_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
float timeSec = 0;
int count = 0;

static void update_time(int h, int m) {

  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "Earth: 00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("Earth: 00:00"), "Earth: %H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("Earth: 00:00"), "Earth: %I:%M", tick_time);
  }

	// Copy date into buffer from tm structure
	static char date_buffer[32];
	strftime(date_buffer, sizeof(date_buffer), "%a, %b %d, %Y", tick_time);
	
	// Create mars buffer
	static char mars_buffer[32];
	char numbers[] = "0123456789";
	int hours = timeSec / 3600;
	int minutes = (timeSec - hours * 3600) / 60;
// 	int seconds = (timeSec - minutes * 60) / 60;
	
	char marsState[] = "--:--";
	if (hours > 12 && clock_is_24h_style() == false) {
		hours -= 12;
	}
		marsState[0] = numbers[hours / 10];
		marsState[1] = numbers[hours % 10];
		marsState[3] = numbers[minutes / 10];
		marsState[4] = numbers[minutes % 10];
	
	strftime(mars_buffer, sizeof(mars_buffer), marsState , tick_time);
	
	// Time to increase per second
// 	timeSec += 0.5316593886462882;
	// Time to increase per minute
	//timeSec += 31.89956331877729;
	timeSec += 31.8996;
	
	// Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
	
	// Show the date
	text_layer_set_text(s_date_layer, date_buffer);
	
	// Show the mars
	text_layer_set_text(s_mars_layer, mars_buffer);
}

static void main_window_load(Window *window) {
	// Create GBitmap, then set to created BitmapLayer
 	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
	window_set_background_color(s_main_window, GColorBlack);
	s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
	
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 130, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "Earth: 00:00");
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	// Create date TextLayer
	s_date_layer = text_layer_create(GRect(0, 145, 144, 50));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, GColorWhite);
	text_layer_set_text(s_date_layer, "Xxx, Xxx 00, 0000");
	text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

	// Create mars TextLayer
	s_mars_layer = text_layer_create(GRect(0, 0, 144, 50));
	text_layer_set_background_color(s_mars_layer, GColorClear);
	text_layer_set_text_color(s_mars_layer, GColorWhite);
	text_layer_set_text(s_mars_layer, "00:00");
	text_layer_set_font(s_mars_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(s_mars_layer, GTextAlignmentCenter);
	
  // Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_mars_layer));
  
  // Make sure the time is displayed from the start
  update_time(0, 0);
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_mars_layer);
	
	// Destroy GBitmap
	gbitmap_destroy(s_background_bitmap);

	// Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	int hours = tick_time->tm_hour;
	int minutes = tick_time->tm_min;
	int seconds = tick_time->tm_sec;
	
	if (count == 0) {
		// Set the initial time of t to be equal to current Earth time in seconds
		timeSec = (hours * 3600) + (minutes * 60) + seconds;
		count += 1;
	}
	update_time(hours, minutes);
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}