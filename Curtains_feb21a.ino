#include <AsyncTimer.h>

#include "arduino_secrets.h"

#include "thingProperties.h"

const int CURTAINS_A_RUN_TIME_MS = 7000;
const int CURTAINS_B_RUN_TIME_MS = 8000;

const int B_STOP = 12;
const int B_OPEN = 10;
const int B_CLOSE = 8;

const int A_STOP = 4;
const int A_OPEN = 6;
const int A_CLOSE = 2;

// For ignoring events sent by Amazon on initial connection. 
int g_initial_change_event_count;

int g_last_timeout_id_A = -1;
int g_last_timeout_id_B = -1;

AsyncTimer g_timer;

void setup() {
  g_initial_change_event_count = 0;
  
  for (int i = 2; i <= 12; i += 2) {
    pinMode(i, OUTPUT);
  }
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  Serial.println("Version 2");

  digitalWrite(A_STOP, HIGH);
  digitalWrite(B_STOP, HIGH);
  delay(500);
  digitalWrite(A_STOP, LOW);
  digitalWrite(B_STOP, LOW);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  
  Serial.println("Setup complete");  
}

void loop() {
  g_timer.handle();
  ArduinoCloud.update();
}

static void pushButton(int button) {
  Serial.print("Pushing button ");
  Serial.println(button, DEC);
  digitalWrite(button, HIGH);
  g_timer.setTimeout([=]() {
    Serial.print("Releasing button ");
    Serial.println(button, DEC);
    digitalWrite(button, LOW);
  }, 500);
}

static bool gotInitialChangeEvent() {
  if (g_initial_change_event_count == 4) {
    return false;
  }
  Serial.println("Treating as initial event");
  if (++g_initial_change_event_count == 4) {
    Serial.println("Got all initial change events");
    digitalWrite(LED_BUILTIN, HIGH);  
  }
  return true;
}

static void processCurtainChange(
    CloudSmartPlug& var,
    int& last_timeout_id,
    int open_button,
    int close_button,
    int stop_button,
    int runtime) {
  if (gotInitialChangeEvent()) {
    return;
  }

  if (last_timeout_id != -1) {
    g_timer.cancel(static_cast<unsigned short>(last_timeout_id));
  }

  if (var) {
    Serial.println("Open curtains");
    pushButton(open_button);
  } else {
    Serial.println("Closing curtains");
    pushButton(close_button);
  }

  last_timeout_id = g_timer.setTimeout([=]() mutable {
    pushButton(stop_button);
    last_timeout_id = -1;
  }, runtime);
}

void onCurtainsAChange()  {
  Serial.println("Got curtains A event");
  processCurtainChange(
    curtains_A,
    g_last_timeout_id_A,
    A_OPEN,
    A_CLOSE,
    A_STOP,
    CURTAINS_A_RUN_TIME_MS);
}

void onCurtainsBChange()  {
  Serial.println("Got curtains B event");
  processCurtainChange(
    curtains_B,
    g_last_timeout_id_B,
    B_OPEN,
    B_CLOSE,
    B_STOP,
    CURTAINS_B_RUN_TIME_MS);
}

void onCurtainsAStopChange() {
  Serial.println("Got curtains A stop event");
  if (gotInitialChangeEvent()) {
    return;
  }
  
  pushButton(A_STOP);
}

void onCurtainsBStopChange() {
  Serial.println("Got curtains B stop event");
  if (gotInitialChangeEvent()) {
    return;
  }
  
  pushButton(B_STOP);
}
