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
