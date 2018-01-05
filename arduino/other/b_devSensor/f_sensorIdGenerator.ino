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

