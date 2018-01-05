#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "HomeWifi";
const char* password = "B0Wkup%3$il4*";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "192.168.1.102";

#define LED_PIN 2

unsigned long tempTime = 0;
const long interval = 10000;

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

// --------- MAIN FUNCTIONS START ---------------
void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  unsigned long timeDelay = millis();
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP8266Client3");
    if (timeDelay  - tempTime >= interval) {
      tempTime = timeDelay;
      client.publish("test", "test msg symfony");
    }
}
// --------- MAIN FUNCTIONS END ---------------

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
}

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
  
  if (topic == "esp8266/3") {
    Serial.print("Changing GPIO 2 to ");
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

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
      YOU  NEED TO CHANGE THIS NEXT LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
      To change the ESP device ID, you will have to give a unique name to the ESP8266.
      Here's how it looks like now:
      if (client.connect("ESP8266Client")) {
      If you want more devices connected to the MQTT broker, you can do it like this:
      if (client.connect("ESPOffice")) {
      Then, for the other ESP:
      if (client.connect("ESPGarage")) {
      That should solve your MQTT multiple connections problem

      THE SECTION IN loop() function should match your device name
    */
    if (client.connect("ESP8266Client3")) {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("esp8266/3");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
