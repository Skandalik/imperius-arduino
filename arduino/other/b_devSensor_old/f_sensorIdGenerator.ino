#include <ESP8266TrueRandom.h>
extern "C"{
 #include "user_interface.h"
}

#define UUID_SIZE 16
#define CHIP_ID_SIZE 4

/**
 * Creates unique UUID for sensor. It's made of generated UUID and added ESP8266 chip ID to it in byte form.
 */
void createSensorId() {
  byte uuid[UUID_SIZE];
  byte chipId[CHIP_ID_SIZE];
  
  ESP8266TrueRandom.uuid(uuid);
  
  Serial.print("UUID: ");
  printByteArray(uuid, UUID_SIZE);
  Serial.println();
 
  setId(chipId);
  
  Serial.print("Chip ID: ");
  printByteArray(chipId, CHIP_ID_SIZE);
  mergeArrays(uuid, chipId);
  Serial.println();
  
  Serial.print("Sensor UUID: ");
  printByteArray(sensorId, 20);

}

/**
 * Prints array of bytes using printHex() method.
 */
void printByteArray(byte* byteArray, int arraySize) {
  for (int i = 0; i < arraySize; i++) {
    printHex(byteArray[i]);
  }
}

/**
 * Prints hex from each byte number which is made up from 2 HEX values
 */
void printHex(byte number) {
  int topDigit = number >> 4;
  int bottomDigit = number & 0x0f;
  // Print high hex digit
  Serial.print(topDigit, HEX);
  // Low hex digit
  Serial.print(bottomDigit, HEX);
}

/**
 * Gets chip ID using system_get_chip_id() method and changes it to byte representation
 * Will be used in UUID generation.
 */
void setId(byte* chipId) {
  int32_t idToConvert = system_get_chip_id();
  convertIdToHex(chipId, idToConvert);
}

/**
 * Converts int number to hex byte array for easier saving into EEPROM.
 */
void convertIdToHex(byte* chipId, uint32_t idToConvert) {
  for (int i = 0; i < CHIP_ID_SIZE; i++) {
    chipId[i] = (idToConvert >> (i*8))  & 0xFF;
  }
}
/**
 * Merges created arrays into one UUID array.
 */
void mergeArrays(byte* uuid, byte* chipId) {
  for (int i = 0; i < 16; i++) {
    sensorId[i] = uuid[i];
  }
  int j = 0;
  for (int i = 16; i < 20; i++) {
    sensorId[i] = chipId[j++];
  }
}
