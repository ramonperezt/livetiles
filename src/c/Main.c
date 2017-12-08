#include <pebble.h>
#include "main.h"


static Window *s_window;

static TextLayer *s_text_time;
static TextLayer *s_text_date;
static TextLayer *s_text_w_lh;
static TextLayer *s_text_temp;
static TextLayer *s_text_steps;
static TextLayer *s_text_cals;
static TextLayer *s_text_sleep;
static TextLayer *s_text_banner;
static TextLayer *s_text_w_details;
static TextLayer *s_text_location;

static BitmapLayer *weather_icon;
static BitmapLayer *l_steps_icon;
static BitmapLayer *l_cal_icon;  
static BitmapLayer *l_sleep_icon; 



static GBitmap *s_bitmap;
static GBitmap *s_foot;
static GBitmap *s_burn;
static GBitmap *s_bed;

static char temperature_buffer[8];
static char temperature_lh_buffer[15];
static char conditions_buffer[15];
static char description_buffer[15];
static char location_buffer[15];
static char news_buffer[85];

ClaySettings settings;

static void prv_default_settings() {
    settings.BackgroundColor = GColorCobaltBlue;
  
  }

static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}



static void prv_update_display() {
  
   if(strcmp(conditions_buffer,"Clouds")==0)
  {
   s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_W_CLOUD);
   bitmap_layer_set_bitmap(weather_icon, s_bitmap);
      
  }
   if(strcmp(conditions_buffer,"Thunderstorm")==0)
  {
    s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_W_STORM);
    bitmap_layer_set_bitmap(weather_icon, s_bitmap);
     
  }
  if(strcmp(conditions_buffer,"Rain")==0)
  {
   
    s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_W_RAIN);
    bitmap_layer_set_bitmap(weather_icon, s_bitmap);
  
    
  }
   if(strcmp(conditions_buffer,"Clear")==0)
  {
    s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_W_SUN);
    bitmap_layer_set_bitmap(weather_icon, s_bitmap);

    
  }
     if(strcmp(conditions_buffer,"Drizzle")==0)
  {
    s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_W_CLOUDSUNRAIN);
    bitmap_layer_set_bitmap(weather_icon, s_bitmap);
  
    
  }
  
  if(strcmp(conditions_buffer,"Fog")==0)
  {
    s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_W_FOG);
    bitmap_layer_set_bitmap(weather_icon, s_bitmap);
 
    
  }

  
  text_layer_set_text(s_text_temp, temperature_buffer);
  text_layer_set_text(s_text_w_details, description_buffer);
  text_layer_set_text(s_text_location, location_buffer);
  text_layer_set_text(s_text_w_lh, temperature_lh_buffer);
  text_layer_set_text(s_text_banner, news_buffer);
  
  //weather icon
  bitmap_layer_set_background_color(weather_icon,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), 
                                      bitmap_layer_get_layer(weather_icon));
  
  //time
  text_layer_set_background_color(s_text_time,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_time));
  
    text_layer_set_background_color(s_text_w_details,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_w_details));
  
  //temperature
  text_layer_set_background_color(s_text_temp,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_temp));
  

    text_layer_set_background_color(s_text_location,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_location));
  
  //weather low high
  text_layer_set_background_color(  s_text_w_lh,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_w_lh));
  
  //date layer
  text_layer_set_background_color(s_text_date,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_date));
  
  //steps layer
  text_layer_set_background_color(s_text_steps,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_steps));
  
  //steps icon
  bitmap_layer_set_background_color( l_steps_icon, settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), 
                                      bitmap_layer_get_layer(l_steps_icon));
  
  //calories layer
  text_layer_set_background_color(s_text_cals,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_cals));
  
  //calories icon
  bitmap_layer_set_background_color(l_cal_icon, settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), 
                                      bitmap_layer_get_layer(l_cal_icon));
  
  //sleep layer
  text_layer_set_background_color(s_text_sleep,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_sleep));
  
  //sleep icon
  bitmap_layer_set_background_color(l_sleep_icon, settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), 
                                      bitmap_layer_get_layer(l_sleep_icon));
  
  //news banner
  text_layer_set_background_color(s_text_banner,  settings.BackgroundColor);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_banner));
  text_layer_enable_screen_text_flow_and_paging(s_text_banner, 5);

}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received"); 


// Read tuples for data
Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
Tuple *tempmin_tuple = dict_find(iterator, MESSAGE_KEY_TEMPMIN);
Tuple *tempmax_tuple = dict_find(iterator, MESSAGE_KEY_TEMPMAX);
Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
Tuple *location_tuple = dict_find(iterator, MESSAGE_KEY_WLOCATION);
Tuple *description_tuple = dict_find(iterator, MESSAGE_KEY_DESCRIPTION);
Tuple *news_tuple = dict_find(iterator, MESSAGE_KEY_NEWS);
Tuple *bg_color_t = dict_find(iterator, MESSAGE_KEY_BackgroundColor);
  
    if(bg_color_t) {
    settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
    }

// If all data is available, use it
if(temp_tuple && conditions_tuple && description_tuple) {
  snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°F", (int)temp_tuple->value->int32);
  snprintf(temperature_lh_buffer, sizeof(temperature_lh_buffer), "L%d°F\nH%d°F", (int)tempmin_tuple->value->int32,(int)tempmax_tuple->value->int32);
  snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
  snprintf(location_buffer, sizeof(location_buffer), "%s", location_tuple->value->cstring);
  snprintf(description_buffer, sizeof(description_buffer), "%s", description_tuple->value->cstring);
  
 // persist_write_string(1,"Demo");
  //persist_read_string(1,test_buffer, sizeof(test_buffer));
   
   persist_write_string(101, temperature_buffer);   
   persist_write_string(102, description_buffer);   
   persist_write_string(103, location_buffer);   
   persist_write_string(104, temperature_lh_buffer);   
   persist_write_string(105, conditions_buffer);   

  APP_LOG(APP_LOG_LEVEL_DEBUG, temperature_lh_buffer); 
}else{
   /*readcache()
   APP_LOG(APP_LOG_LEVEL_DEBUG, "Right way"); 
   APP_LOG(APP_LOG_LEVEL_DEBUG, temperature_buffer);*/ 
}
  
if(news_tuple) {
  snprintf(news_buffer, sizeof(news_buffer), "%s", news_tuple->value->cstring); 
  
  persist_write_string(106, news_buffer);
}else{
  //persist_read_string(106,news_buffer, sizeof(news_buffer));
  
}
  


  
 
  prv_save_settings();  
	

}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  static char s_date[12];
  strftime(s_buffer, sizeof(s_buffer),  "%I:%M%p", tick_time);
  strftime(s_date, sizeof(s_date),  "%Y %m/%d", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_text_time, s_buffer);
  text_layer_set_text(s_text_date, s_date);
}

static void update_health() {
  // Get a tm structure
  static char steps_buffer[6];
  static char cals_buffer[6];
  static char sleep_buffer[6];
  int sleephours= (health_service_sum_today(HealthMetricSleepSeconds)/60)/60;
  
  snprintf(steps_buffer, sizeof(steps_buffer), "\n%d", (int)health_service_sum_today(HealthMetricStepCount));
  snprintf(cals_buffer, sizeof(cals_buffer), "\n%d", (int)health_service_sum_today(HealthMetricActiveKCalories)+(int)health_service_sum_today(HealthMetricRestingKCalories));
  snprintf(sleep_buffer, sizeof(sleep_buffer), "\n%dH", sleephours);
  
  text_layer_set_text(s_text_steps, steps_buffer);
  text_layer_set_text(s_text_cals, cals_buffer);
  text_layer_set_text(s_text_sleep, sleep_buffer);



}

static void tick_handler_minute(struct tm *tick_time, TimeUnits units_changed) {
 update_time();
 update_health();
}

static void readcache(){
  
    persist_read_string(101,temperature_buffer, sizeof(temperature_buffer));
   persist_read_string(102,description_buffer, sizeof(description_buffer));
   persist_read_string(103,location_buffer, sizeof(location_buffer));
   persist_read_string(104,temperature_lh_buffer, sizeof(temperature_lh_buffer));
   persist_read_string(105,conditions_buffer, sizeof(conditions_buffer));
  persist_read_string(106,news_buffer, sizeof(news_buffer));
   APP_LOG(APP_LOG_LEVEL_DEBUG, "Fron cache"); 
  
  
}


static void init(void) {
	// Open AppMessage
  readcache();
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
  app_message_register_inbox_received(inbox_received_callback);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler_minute);
  
  //Create window
  s_window = window_create();
 	window_set_background_color(s_window, GColorBlack);
  
  //weather icon
  weather_icon = bitmap_layer_create(GRect(0, 0, 46, 40));
  bitmap_layer_set_compositing_mode(weather_icon, GCompOpSet);
  bitmap_layer_set_background_color(weather_icon,  settings.BackgroundColor);
   

  

  //time
  s_text_time = text_layer_create(GRect(48, 0, 94, 40));
  text_layer_set_font(s_text_time, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_text_time, GTextAlignmentCenter);
  text_layer_set_text_color(s_text_time,  GColorWhite);

    //weather details
  s_text_w_details = text_layer_create(GRect(48, 28, 94, 12));
  text_layer_set_font(s_text_w_details, fonts_get_system_font(FONT_KEY_GOTHIC_09));
  text_layer_set_text_color(s_text_w_details,  GColorWhite);
  text_layer_set_text_alignment(s_text_w_details, GTextAlignmentCenter);
  
  //Weather Temperature
  s_text_temp = text_layer_create(GRect(0, 42, 46, 40));
  text_layer_set_font(s_text_temp, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(s_text_temp,  GColorWhite);
  text_layer_set_text_alignment(s_text_temp, GTextAlignmentCenter);
  
  s_text_location = text_layer_create(GRect(0, 70, 46, 10));
  text_layer_set_font(s_text_location, fonts_get_system_font(FONT_KEY_GOTHIC_09));
  text_layer_set_text_color(s_text_location,  GColorWhite);
  text_layer_set_text_alignment(s_text_location, GTextAlignmentCenter);

 
  
  
  //Weather Low high
  s_text_w_lh = text_layer_create(GRect(48, 42, 46, 40));
  text_layer_set_text_color(  s_text_w_lh,  GColorWhite);
  text_layer_set_font(s_text_w_lh, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment (s_text_w_lh, GTextAlignmentCenter);
  
   //Date Layer
  s_text_date = text_layer_create(GRect(96, 42, 46, 40));
  text_layer_set_text_color(s_text_date,  GColorWhite);
  text_layer_set_font(s_text_date, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment (s_text_date, GTextAlignmentCenter);
  
  //Steps Layer
  s_text_steps = text_layer_create(GRect(0, 84, 46, 40));
  text_layer_set_text_color(s_text_steps,  GColorWhite);
  text_layer_set_font(s_text_steps, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment (s_text_steps, GTextAlignmentCenter);

    
  //Steps Icon
  l_steps_icon = bitmap_layer_create(GRect(15, 84, 12, 20));
  bitmap_layer_set_compositing_mode( l_steps_icon, GCompOpSet);
  s_foot=gbitmap_create_with_resource(RESOURCE_ID_FOOT);
  bitmap_layer_set_bitmap(l_steps_icon,s_foot);
  
  //Calories Layer
  s_text_cals = text_layer_create(GRect(48, 84, 46, 40));
  text_layer_set_text_color(s_text_cals,  GColorWhite);
  text_layer_set_font(s_text_cals, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment (s_text_cals, GTextAlignmentCenter);

  
  //Calories Icon
  l_cal_icon = bitmap_layer_create(GRect(64, 84, 12, 20));
  bitmap_layer_set_compositing_mode(l_cal_icon, GCompOpSet);
  s_burn=gbitmap_create_with_resource(RESOURCE_ID_BURN);
  bitmap_layer_set_bitmap(l_cal_icon,s_burn);
  
  //Sleep Layer
  s_text_sleep = text_layer_create(GRect(96, 84, 46, 40));
  text_layer_set_text_color(s_text_sleep,  GColorWhite);

  text_layer_set_font(s_text_sleep, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment (s_text_sleep, GTextAlignmentCenter);
  
  
  
    //Sleep Icon
  l_sleep_icon = bitmap_layer_create(GRect(111, 84, 12, 20));
  bitmap_layer_set_compositing_mode(l_sleep_icon, GCompOpSet);
  s_bed=gbitmap_create_with_resource(RESOURCE_ID_SLEEP);
  bitmap_layer_set_bitmap(l_sleep_icon,s_bed);

 
  //Banner
   s_text_banner = text_layer_create(GRect(0, 126, 142, 42));
  text_layer_set_text_color(s_text_banner,  GColorWhite);
  
  text_layer_set_text_alignment (s_text_banner, GTextAlignmentLeft);
  text_layer_set_font(s_text_banner, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_text_banner, "Loading...");
  
 
  prv_load_settings();
  prv_update_display();
  
  window_stack_push(s_window, true);
		
}

static void deinit(void) {
	// Destroy the text layer
	
  text_layer_destroy(s_text_time);
  text_layer_destroy(s_text_date);
  text_layer_destroy(s_text_w_lh);
  text_layer_destroy(s_text_temp);
  text_layer_destroy(s_text_cals);
  text_layer_destroy(s_text_sleep);
  text_layer_destroy(s_text_steps);
  text_layer_destroy(s_text_banner);
  text_layer_destroy(s_text_w_details);
  text_layer_destroy(s_text_location);
 
  //destroy the bitmaps
  bitmap_layer_destroy(weather_icon);
  bitmap_layer_destroy(l_steps_icon);
  bitmap_layer_destroy(l_cal_icon);
  bitmap_layer_destroy(l_sleep_icon); 
  
  gbitmap_destroy(s_bitmap);
  gbitmap_destroy(s_foot);
  gbitmap_destroy(s_burn);
  gbitmap_destroy(s_bed);
	
	// Destroy the window
	window_destroy(s_window);
  
}





int main(void) {
	init();
	app_event_loop();
	deinit();
}

