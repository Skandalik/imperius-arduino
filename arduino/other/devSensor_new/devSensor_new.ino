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

/*
 * SENSOR ID HANDLER
 */
void handleSensorId() {
  byte sensorIdFlag = SENSOR_ID_INIT_FLAG;

//  EEPROM.begin(1024);
//  sensorIdFlag = EEPROM.read(addressPointer);
 
  Serial.print("UUID flag:");
  Serial.println(sensorIdFlag);
  Serial.println();

  if (sensorIdFlag == 0) {
//    clearSensorEEPROM();
//    saveFlagOnEeprom(sensorIdFlag);
    createSensorId();
    
    Serial.print("Sensor ID to save in EEPROM:");
//    Serial.println(sensorIdToEeprom);
    
//    EEPROM.put(addressPointer, sensorIdToEeprom);
//    EEPROM.commit();
  } else {
//    Serial.print("Reading sensor ID from EEPROM");
    
//    addressPointer++;
//    EEPROM.get(addressPointer, sensorIdToEeprom);
//    addressPointer += sizeof(sensorIdToEeprom);
  }
  EEPROM.end();

  Serial.println();
  Serial.print("Sensor ID:");
  Serial.println(sensorId);
}

void saveFlagOnEeprom(byte sensorIdFlag) {
  sensorIdFlag = SENSOR_ID_INIT_FLAG;
  Serial.println("Setting SensorID flag to 1.");
  EEPROM.write(addressPointer, sensorIdFlag);

  addressPointer++;
}

/*
 * END SENSOR ID HANDLER
 */

/*
 * EEPROM HANDLER
 */

 /**
 * Clears EEPROM memory.
 * Must be used after EEPROM.begin() and must use EEPROM.end()
 */
void clearSensorEEPROM() {
  Serial.println("Clearing EEPROM memory.");
  for (int i = 0; i < 1024; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("Cleared");
}

/*
 * END EEPROM HANDLER
 */

/*
 * SENSOR ID GENERATOR
 */

#include <ESP8266TrueRandom.h>

#define UUID_SIZE 16

/**
 * Creates unique UUID for sensor.
 * It's made of generated UUID and added ESP8266 chip ID to it in byte form.
 */
void createSensorId() {
  Serial.println("------------------");
  Serial.println("Sensor ID creation");
  Serial.println("------------------");
  Serial.println();
  
  byte uuidNumber[UUID_SIZE];
  String chipId = String(ESP.getChipId(), HEX);
  
  ESP8266TrueRandom.uuid(uuidNumber);
  appendToString(uuidNumber, UUID_SIZE);
  Serial.print("UUID: ");
  Serial.println(sensorId);

  sensorId += chipId;
  sensorId.toUpperCase();
  Serial.print("Chip ID: ");
  Serial.println(chipId);
  
  Serial.print("Sensor UUID: ");
  Serial.println(sensorId);
  sensorId.toCharArray(sensorIdToEeprom, SENSOR_ID_SIZE);

  Serial.println();
  Serial.println("---------------------------");
  Serial.println("Finished creating Sensor ID");
  Serial.println("---------------------------");
  Serial.println();
}

/**
 * Appends to String
 */
void appendToString(byte* byteArray, int arraySize) {
  for (int i = 0; i < arraySize; i++) {
    addHexToString(byteArray[i]);
  }
}

/**
 * Gets first and second part of byte as integers,
 * then passes to String constructor with HEX parameter which
 * converts input variable to HEX value and then it's appended
 * to global String variable.
 */
void addHexToString(byte number) {
  sensorId += String(getFirstHexDigit(number), HEX);
  sensorId += String(getSecondHexDigit(number), HEX);
}

/**
 * Gets first 4 bits of byte which are first hex digit
 * and returns an integer
 */
int getFirstHexDigit(byte number) {
  return number >> 4;
}

/**
 * Gets second 4 bits of byte which are second hex digit
 * and returns an integer
 */
int getSecondHexDigit(byte number) {
  return number & 0x0F;
}

/*
 * END SENSOR ID GENERATOR
 */

/*
 * WIFI HANDLER
 */

 void setupWifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}
/*
 * END WIFI HANDLER
 */

 /*
  * MQTT HANDLER
  */

  String registeredTopic ="";

String setStatusTopic ="";
String allCheckStatusTopic ="";
String checkStatusResponseTopic= "";

void setupMqtt() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  registeredTopic += "sensor/";
  registeredTopic += sensorIdToEeprom;
  registeredTopic += "/registered";
  
  checkStatusResponseTopic += "sensor/";
  checkStatusResponseTopic += sensorIdToEeprom;
  checkStatusResponseTopic += "/status/response";

  allCheckStatusTopic += "sensor/all/status/check";
  
  setStatusTopic += "sensor/";
  setStatusTopic += sensorIdToEeprom;
  setStatusTopic += "/status/set";
}

void callback(String topic, byte* message, unsigned int length) {
  String messageTemp;
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print(". Message: ");

  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  
  Serial.println(messageTemp);
  Serial.println();
  
  JsonObject& sensor = jsonBuffer.createObject();
  char jsonChar[100] = "";

  if (topic == registeredTopic.c_str() ) {
    registeredFlag = true;
    client.unsubscribe(registeredTopic.c_str());
    Serial.println("Sensor has been registered");
  }

  
  if (topic == allCheckStatusTopic.c_str() ) {
    int currentStatus = digitalRead(ledPin);
    sensor["uuid"] = sensorId.c_str();
    sensor["status"] = currentStatus;
    sensor["action"] = "update";
    sensor.printTo((char*)jsonChar, sensor.measureLength() + 1);
    client.publish("sensor/response", jsonChar); 
    jsonBuffer.clear();
  }
  
  
  if (topic == setStatusTopic.c_str() ) {
    if (messageTemp == "1") {
      digitalWrite(ledPin, HIGH);
      Serial.print("ON");
    }
    else if (messageTemp == "0") {
      digitalWrite(ledPin, LOW);
      Serial.print("Off");
    }
  }
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(sensorIdToEeprom)) {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      
      client.subscribe(registeredTopic.c_str());
      client.subscribe(setStatusTopic.c_str());
      client.subscribe(allCheckStatusTopic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/*
 * END MQTT HANDLER
 */
