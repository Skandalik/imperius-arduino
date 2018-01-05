#define UUID_SIZE 16
#define CHIP_ID_SIZE 4

/**
 * Creates unique UUID for sensor. It's made of generated UUID and added ESP8266 chip ID to it in byte form.
 */
 String uuidString;
void createSensorId() {
  byte chipUuid[UUID_SIZE];
  uint32_t chipId;
  
  TrueRandom.uuid(chipUuid);
  
  Serial.print("UUID: ");
//  appendUuidToString(chipUuid, UUID_SIZE);
  Serial.println();
 
  chipId = getId();

  for(int i= 0 ; i < CHIP_ID_SIZE; i++) {
    Serial.print(chipId >> (i*8), HEX);
  }
  Serial.println();
  Serial.print("Chip ID: ");
  Serial.println(chipId);
  
  Serial.print("Sensor UUID: ");
  uuidString += chipId;
  uuidString.toUpperCase();
  Serial.println(uuidString);

}



/**
 * Gets chip ID using system_get_chip_id() method and changes it to byte representation
 * Will be used in UUID generation.
 */
uint32_t getId() {
  return 2086251;
}
