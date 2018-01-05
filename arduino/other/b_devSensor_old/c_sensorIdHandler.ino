void handleSensorId() {
  clearSensorEEPROM();
  byte sensorIdFlag = SENSOR_ID_INIT_FLAG;
  
  EEPROM.begin(1024);
  sensorIdFlag = EEPROM.read(SENSOR_ID_FLAG_ADDRESS);
  Serial.println();
  Serial.print("UUID flag address:");
  Serial.println(SENSOR_ID_FLAG_ADDRESS);
  Serial.print("UUID flag:");
  Serial.println(sensorIdFlag);
  
  if (sensorIdFlag == 0) {
//    clearSensorEEPROM();
    sensorIdFlag = SENSOR_ID_INIT_FLAG;
    Serial.println("Setting id flag to 1.");
    EEPROM.write(SENSOR_ID_FLAG_ADDRESS, sensorIdFlag);
    createSensorId();
    writeSensorIdToEEPROM();
    EEPROM.commit();
  } else {
    readSensorIdFromEEPROM(sensorEepromAddressPointer, SENSOR_ID_SIZE);
  }

  Serial.print("Sensor UUID: ");
  printByteArray(sensorId, 20);
  EEPROM.end();
}
