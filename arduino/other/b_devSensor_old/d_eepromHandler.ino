void writeSensorIdToEEPROM() {
  Serial.println("Saving ID in sensor EEPROM memory.");
  for (int i = sensorEepromAddressPointer; i < SENSOR_ID_SIZE; i++) {
    EEPROM.write(i, sensorId[i]);
  }
  Serial.println("Finished saving to EEPROM.");
}

void readSensorIdFromEEPROM(int startingPointAddress, int finishPointAddress) {
  Serial.println("Reading id from EEPROM");
  for (int i = startingPointAddress; i < finishPointAddress; i++) {
    sensorId[i] = EEPROM.read(i);
  }
}

void clearSensorEEPROM() {
  EEPROM.begin(1024);
  Serial.print("Clearing EEPROM memory.");
  for (int i = 0; i < 1024; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.print("Cleared");
  EEPROM.end();
}

