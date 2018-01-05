#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266TrueRandom.h>

// CONSTANTS
#define UUID_SIZE 16
#define SENSOR_ID_SIZE 70
#define SENSOR_ID_INIT_FLAG 1
#define LED_PIN 2

// GLOBAL VARIABLES
// Address pointer and sensor ID variables
int addressPointer = 0;
String sensorId;
char sensorIdToEeprom[SENSOR_ID_SIZE];

// Flag variables
boolean registeredFlag = false;
boolean switchable = true;
boolean adjustable = true;
boolean fetchable = false;
int sensorStatus;

// WiFi credentials
const char *ssid = "HomeWifi";
const char *password = "B0Wkup%3$il4*";

// MQTT Server address
const char *mqtt_server = "192.168.1.102";

// OBJECTS DECLARATION
WiFiClient wifiClient;
PubSubClient client(wifiClient);
StaticJsonBuffer<200> jsonBuffer;

// ADDITIONAL VARIABLES
unsigned long tempTime = 0;
const long interval = 10000;

// TOPICS
String TOPIC_PREFIX = "sensor/";
String responseTopic = TOPIC_PREFIX + "response";
String registeredTopic;
String setStatusTopic;
String allCheckStatusTopic;
String checkStatusResponseTopic;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  sensorStatus = digitalRead(LED_PIN);

  createSensorId();
  //  handleSensorId();
  setupWifi();
  setupMqtt();
}

void loop()
{
  unsigned long timeDelay = millis();
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
    client.connect(sensorIdToEeprom);

  if ((timeDelay - tempTime >= interval) && !registeredFlag)
  {
    tempTime = timeDelay;
    sendRegisterBroadcastMessage(client);
  }
}

// Sending register message every 10 seconds (can be changed)
void sendRegisterBroadcastMessage(PubSubClient client) {
  JsonObject &sensorInit = jsonBuffer.createObject();
  char jsonCharInit[200] = "";
  String ip = WiFi.localIP().toString();
  
    sensorInit["uuid"] = sensorId.c_str();
    sensorInit["switchable"] = switchable;
    sensorInit["adjustable"] = adjustable;
    sensorInit["status"] = sensorStatus;
    sensorInit["ip"] = ip.c_str();
    sensorInit["action"] = "register";

  sensorInit.printTo((char *)jsonCharInit, sensorInit.measureLength() + 1);

  client.publish("sensor/register", jsonCharInit);
  jsonBuffer.clear();
}

// SENSOR ID HANDLER
void handleSensorId()
{
  byte sensorIdFlag = SENSOR_ID_INIT_FLAG;

  EEPROM.begin(1024);
  sensorIdFlag = EEPROM.read(addressPointer);

  if (sensorIdFlag == 0)
  {
    clearSensorEEPROM();
    saveFlagOnEeprom(sensorIdFlag);
    createSensorId();

    EEPROM.put(addressPointer, sensorIdToEeprom);
    EEPROM.commit();
  }
  else
  {
    addressPointer++;
    EEPROM.get(addressPointer, sensorIdToEeprom);
    addressPointer += sizeof(sensorIdToEeprom);
  }
  EEPROM.end();
}

void saveFlagOnEeprom(byte flag)
{
  flag = SENSOR_ID_INIT_FLAG;
  EEPROM.write(addressPointer, flag);

  addressPointer++;
}

/*
 * END SENSOR ID HANDLER
 */

/**
 * Clears EEPROM memory.
 * Must be used after EEPROM.begin() and must use EEPROM.end()
 */
void clearSensorEEPROM()
{
  Serial.println("Clearing EEPROM memory.");
  for (int i = 0; i < 1024; i++)
  {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("Cleared");
}

// SENSOR ID GENERATOR
/**
   * Creates unique UUID for sensor.
   * It's made of generated UUID and added ESP8266 chip ID to it in byte form.
   */
void createSensorId()
{
  byte uuidNumber[UUID_SIZE];
  String chipId = String(ESP.getChipId(), HEX);

  ESP8266TrueRandom.uuid(uuidNumber);
  appendToString(uuidNumber, UUID_SIZE);

  sensorId += chipId;
  sensorId.toUpperCase();
  sensorId.toCharArray(sensorIdToEeprom, SENSOR_ID_SIZE);
}

/**
   * Appends to String
   */
void appendToString(byte *byteArray, int arraySize)
{
  for (int i = 0; i < arraySize; i++)
  {
    addHexToString(byteArray[i]);
  }
}

/**
   * Gets first and second part of byte as integers,
   * then passes to String constructor with HEX parameter which
   * converts input variable to HEX value and then it's appended
   * to global String variable.
   */
void addHexToString(byte number)
{
  sensorId += String(getFirstHexDigit(number), HEX);
  sensorId += String(getSecondHexDigit(number), HEX);
}

/**
   * Gets first 4 bits of byte which are first hex digit
   * and returns an integer
   */
int getFirstHexDigit(byte number)
{
  return number >> 4;
}

/**
   * Gets second 4 bits of byte which are second hex digit
   * and returns an integer
   */
int getSecondHexDigit(byte number)
{
  return number & 0x0F;
}

// END SENSOR ID GENERATOR

// WIFI HANDLER
void setupWifi()
{
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Serial.println(WiFi.localIP());
}

// MQTT HANDLER
void setupMqtt()
{
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  registeredTopic = TOPIC_PREFIX + sensorIdToEeprom + "/registered";
  checkStatusResponseTopic = TOPIC_PREFIX + sensorIdToEeprom + "/status/response";
  allCheckStatusTopic = TOPIC_PREFIX + "all/status/check";
  setStatusTopic = TOPIC_PREFIX + sensorIdToEeprom + "/status/set";
}

// CALLBACK FOR MESSAGE
void callback(String topic, byte *message, unsigned int length)
{
  String messageTemp;
  JsonObject &sensor = jsonBuffer.createObject();
  char jsonChar[200] = "";

  // Decodes message from bytes to String
  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)message[i];
  }

  if (topic.equals(registeredTopic))
  {
    registeredFlag = true;
    client.unsubscribe(registeredTopic.c_str());
  }

  if (topic.equals(allCheckStatusTopic))
  {
    int currentStatus = digitalRead(LED_PIN);

    sensor["uuid"] = sensorId.c_str();
    sensor["status"] = currentStatus;
    sensor["action"] = "update";

    sensor.printTo((char *)jsonChar, sensor.measureLength() + 1);
    client.publish(responseTopic.c_str(), jsonChar);
    jsonBuffer.clear();
  }

  if (topic.equals(setStatusTopic))
  {
    if (messageTemp == "1")
    {
      digitalWrite(LED_PIN, HIGH);
    }
    else if (messageTemp == "0")
    {
      digitalWrite(LED_PIN, LOW);
    }
  }
  jsonBuffer.clear();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(sensorIdToEeprom))
    {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)

      client.subscribe(registeredTopic.c_str());
      client.subscribe(setStatusTopic.c_str());
      client.subscribe(allCheckStatusTopic.c_str());
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
