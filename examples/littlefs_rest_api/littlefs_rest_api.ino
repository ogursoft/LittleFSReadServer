#define DEBUG_SERIAL //uncomment for Serial debugging statements

#ifdef DEBUG_SERIAL
#define DEBUG_BEGIN Serial.begin(115200)
#define DEBUG_PRINT(x) Serial.println(x)
#else
#define DEBUG_PRINT(x) 
#define DEBUG_BEGIN
#endif

//includes
#include <PersWiFiManager.h> // http://ryandowning.net/PersWiFiManager
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266SSDP.h>

//extension of ESP8266WebServer with LittleFS handlers built in
#include <LittleFSReadServer.h> // https://github.com/ogursoft/LittleFSReadServer
// upload data folder to chip with Arduino ESP8266 filesystem uploader
// https://github.com/esp8266/arduino-esp8266fs-plugin

#include <DNSServer.h>
#include <FS.h>
#include <LittleFS.h>

#define DEVICE_NAME "ESP8266 DEVICE"


//server objects
//just use this instead of "ESP8266WebServer server(80);"
LittleFSReadServer server(80);
DNSServer dnsServer;
PersWiFiManager persWM(server, dnsServer);

////// Sample program data
int x;
String y;

void setup() {
  DEBUG_BEGIN; //for terminal debugging
  DEBUG_PRINT();
  
  //optional code handlers to run everytime wifi is connected...
  persWM.onConnect([]() {
    DEBUG_PRINT("wifi connected");
    DEBUG_PRINT(WiFi.localIP());
    EasySSDP::begin(server, DEVICE_NAME);
  });
  //...or AP mode is started
  persWM.onAp([](){
    DEBUG_PRINT("AP MODE");
    DEBUG_PRINT(persWM.getApSsid());
  });

  //allows serving of files from LittleFS
  LittleFS.begin();
  //sets network name for AP mode
  persWM.setApCredentials(DEVICE_NAME);
  //persWM.setApCredentials(DEVICE_NAME, "password"); optional password
  persWM.begin();

  //handles commands from webpage, sends live data in JSON format
  server.on("/api", []() {
    DEBUG_PRINT("server.on /api");
    if (server.hasArg("x")) {
      x = server.arg("x").toInt();
      DEBUG_PRINT(String("x: ")+x);
    } //if
    if (server.hasArg("y")) {
      y = server.arg("y");
      DEBUG_PRINT("y: "+y);
    } //if

    //build json object of program data
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["x"] = x;
    json["y"] = y;

    char jsonchar[200];
    json.printTo(jsonchar); //print to char array, takes more memory but sends in one piece
    server.send(200, "application/json", jsonchar);

  }); //server.on api

  server.begin();
  DEBUG_PRINT("setup complete.");
} //void setup

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  // do stuff with x and y

} //void loop
