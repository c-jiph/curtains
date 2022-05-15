// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)

void onCurtainsAChange();
void onCurtainsAStopChange();
void onCurtainsBChange();
void onCurtainsBStopChange();

CloudSmartPlug curtains_A;
CloudSmartPlug curtains_A_Stop;
CloudSmartPlug curtains_B;
CloudSmartPlug curtains_B_Stop;

void initProperties(){

  ArduinoCloud.addProperty(curtains_A, READWRITE, ON_CHANGE, onCurtainsAChange);
  ArduinoCloud.addProperty(curtains_A_Stop, READWRITE, ON_CHANGE, onCurtainsAStopChange);
  ArduinoCloud.addProperty(curtains_B, READWRITE, ON_CHANGE, onCurtainsBChange);
  ArduinoCloud.addProperty(curtains_B_Stop, READWRITE, ON_CHANGE, onCurtainsBStopChange);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
