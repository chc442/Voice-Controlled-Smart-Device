
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"


#define LED                             2

// prototypes
boolean connectWifi();

//on/off callbacks 
bool LEDOn();
bool LEDOff();
bool magicGlassOn();
bool magicGlassOff();

// wifi credentials
// HDC wifi
const char* ssid = "guestnet";
const char* password = "93Tr1D1uM45";


boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *ledSwitch = NULL;
Switch *magicGlass = NULL;

bool isLEDOn = false;
bool isMagicGlassOn = false;

void setup()
{

  // LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH); // negative logic. HIGH=off

  // set up digital IO
  pinMode(D1,OUTPUT);
  digitalWrite(D1,HIGH); // negative logic
    
  Serial.begin(9600);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 10
    // Format: Alexa invocation name, local port no, on callback, off callback
    ledSwitch = new Switch("LED", 80, LEDOn, LEDOff);
    magicGlass = new Switch("Magic Glass", 81, magicGlassOn, magicGlassOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*ledSwitch);
    upnpBroadcastResponder.addDevice(*magicGlass);
  }
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      ledSwitch->serverLoop();
      magicGlass->serverLoop();
	 }
   /*
   else{ // if not connected to wifi
       // Initialise wifi connection
       wifiConnected = connectWifi();
   }
   */
       
   
}

bool LEDOn() {
    Serial.println("LED turn on ...");
    digitalWrite(LED, LOW); // turn on LED
    isLEDOn = true;    
    return isLEDOn;
}

bool LEDOff() {
    Serial.println("LED turn off ...");
    digitalWrite(LED, HIGH); // turn off LED
    isLEDOn = false;
    return isLEDOn;
}

bool magicGlassOn() {
    Serial.println("Magic Glass turn on ...");
    digitalWrite(D1,LOW);
    isMagicGlassOn = true;
    return isMagicGlassOn;
}

bool magicGlassOff() {
  Serial.println("Magic Glass turn off ...");
  digitalWrite(D1,HIGH);
  isMagicGlassOn = false;
  return isMagicGlassOn;
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.setPhyMode(WIFI_PHY_MODE_11N); // THIS IS REQUIRED TO CONNECT TO HDC'S HIDDEN WIFI
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}
