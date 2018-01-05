#include <ArduinoJson.h>

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>



#define SENSOR_ID_SIZE 70
#define SENSOR_ID_INIT_FLAG 1

/**
 * EEPROM pointers, flags and variables
 */
int addressPointer = 0;
String sensorId;
char sensorIdToEeprom[SENSOR_ID_SIZE];
boolean registeredFlag = false;
boolean switchable = true;
boolean adjustable = false;
boolean fetchable = false;
int sensorStatus = 1;


/**
 * MQTT variables
 */
 // WiFi credentials
const char* ssid = "HomeWifi";
const char* password = "B0Wkup%3$il4*";

// MQTT Broker address
const char* mqtt_server = "192.168.1.102";


/**
 * Declaring objects
 */
WiFiClient wifiClient;
PubSubClient client(wifiClient);
StaticJsonBuffer<200> jsonBuffer;


/**
 * PINS
 */

int ledPin = 2;

unsigned long tempTime = 0;
const long interval = 10000;

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  pinMode(ledPin, OUTPUT);

  createSensorId();
//  handleSensorId();
  setupWifi();
  setupMqtt();

  
}

void loop() {
  unsigned long timeDelay = millis();
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect(sensorIdToEeprom);
    
    if ((timeDelay  - tempTime >= interval) && !registeredFlag) {
        JsonObject& sensorInit = jsonBuffer.createObject();
        char jsonCharInit[200] = "";
//      if (!registeredFlag) {
        Serial.println("Registering sensor");
        tempTime = timeDelay;
        String ip = WiFi.localIP().toString();
        sensorInit["uuid"] = sensorId.c_str();
        sensorInit["switchable"] = switchable;
        sensorInit["adjustable"] = adjustable;
        sensorInit["status"] = sensorStatus;
        sensorInit["ip"] = ip.c_str();
        sensorInit["action"] = "register";
        sensorInit.printTo((char*)jsonCharInit, sensorInit.measureLength() + 1);
        Serial.println(ip.c_str());
        Serial.println(jsonCharInit);
        
        client.publish("sensor/register", jsonCharInit);
        jsonBuffer.clear();
//      }
    }
}

