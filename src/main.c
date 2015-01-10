#include <pebble.h>

static Window *sMainWindow;
static TextLayer *sOutputLayer;

static void compassHandler(CompassHeadingData data){
  // Output buffer
  static char sBuffer[32];
  
  switch (data.compass_status){
    case CompassStatusDataInvalid:
      text_layer_set_text(sOutputLayer, "Move watch to calibrate");
      break;    
    
    case CompassStatusCalibrating:
      snprintf(sBuffer, sizeof(sBuffer), "Compass\nCalibrating\n\nHeading: %d", TRIGANGLE_TO_DEG((int)data.true_heading));
      text_layer_set_text(sOutputLayer, sBuffer);
      break;
    
    case CompassStatusCalibrated:
      snprintf(sBuffer, sizeof(sBuffer), "Compass\nCalibrated\n\nHeading: %d", TRIGANGLE_TO_DEG((int)data.true_heading));
      text_layer_set_text(sOutputLayer, sBuffer);
      break;
    
    default:
      text_layer_set_text(sOutputLayer, "Compass Data Unknown");
      break;
  }
}

static void mainWindowLoad(Window *window){
  // Handles loading the contents inside the window
  Layer *windowLayer = window_get_root_layer(window);
  GRect windowBounds = layer_get_bounds(windowLayer);
  
  // Create the text layer
  sOutputLayer = text_layer_create(GRect(0, 0, windowBounds.size.w, windowBounds.size.h));
  
  // Set visuals of text layer
  text_layer_set_background_color(sOutputLayer, GColorBlack);
  text_layer_set_text_color(sOutputLayer, GColorClear);
  text_layer_set_font(sOutputLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  
  text_layer_set_text(sOutputLayer, "Calibrating...");
  text_layer_set_text_alignment(sOutputLayer, GTextAlignmentCenter);
  layer_add_child(windowLayer, text_layer_get_layer(sOutputLayer));
}

static void mainWindowUnload(Window *window){
  // Handles unloading the contents inside the window
  text_layer_destroy(sOutputLayer);
}

static void init(){
  // Initializes the watch app
  
  // Assign the pointer to the window
  sMainWindow = window_create();
  
  // Set the window colour to black
  window_set_background_color(sMainWindow, GColorBlack);
  
  // Set up window handlers
  window_set_window_handlers(sMainWindow, (WindowHandlers){
    .load = mainWindowLoad,
    .unload = mainWindowUnload
  });
  
  // Shows the window on the watch
  // The second parameter specifies if animation should be enabled
  window_stack_push(sMainWindow, true);
  
  // Subscribe to compass events
  //update when the angle changes by more than 10 degrees
  compass_service_subscribe(compassHandler);
  compass_service_set_heading_filter(TRIG_MAX_ANGLE / 36);
}

static void deinit(){
  // Unloads the watch app when switching to a new task
  
  // Destroy the window to free memory
  window_destroy(sMainWindow);
  
  // Unsubscribe to compass events
  compass_service_unsubscribe();
}

int main(void){
  init();
  app_event_loop();
  deinit();
}