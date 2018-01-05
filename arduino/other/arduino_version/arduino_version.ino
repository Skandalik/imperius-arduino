#include <EEPROM.h>
#include <PubSubClient.h>
#include <TrueRandom.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  createSensorId();
}

void loop() {

}

