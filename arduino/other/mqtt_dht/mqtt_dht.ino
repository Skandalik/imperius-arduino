#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <stdio.h>
#include <EEPROM.h>


// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "HomeWifi";
const char* password = "B0Wkup%3$il4*";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "192.168.1.102";

#define LED_PIN 0
#define DHT_PIN 2
#define DHT_TYPE DHT11

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHT_PIN, DHT_TYPE);
float temperature;
float humidity;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
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

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
//
//  // Feel free to add more if statements to control more GPIOs with MQTT
//
//  // If a message is received on the topic home/office/esp1/gpio2, you check if the message is either 1 or 0. Turns the ESP GPIO according to the message
  if (topic == "scan/2") {
    if (messageTemp == "1") {
      digitalWrite(LED_PIN, HIGH);
      delay(10);
      digitalWrite(LED_PIN, LOW);
      Serial.println("Measuring temp and humidity: ");
      temperature = dht.readTemperature(true);
      Serial.println();
      Serial.print("Temperature: ");
      Serial.println(temperature);
      Serial.println("Publishing temp");
      char message[10] = "";
      dtostrf(temperature, 3, 2, message);
      Serial.println("Temp before publishing: ");
      Serial.println(message);
      client.publish("esp8266/temperature", message);
    }
//    Serial.print("Humidity: ");
//    Serial.println(humidity);
  }
  if (topic == "esp8266/0") {
    Serial.print("Changing GPIO 0 to ");
    if (messageTemp == "1") {
      digitalWrite(LED_PIN, HIGH);
      Serial.print("On");
    }
    else if (messageTemp == "0") {
      digitalWrite(LED_PIN, LOW);
      Serial.print("Off");
    }
  }
  Serial.println();
}


// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually chontrols the LEDs
void setup() {
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// For this project, you don't need to change anything in the loop function.
// Basically it ensures that you ESP is connected to your broker
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect("DevSensor");
}

