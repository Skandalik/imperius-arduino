#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(1000);
//  clearSensorEEPROM();
//  createSensorId();
  handleSensorId();
}

void loop() {

}

