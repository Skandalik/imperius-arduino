/**
 * Clears EEPROM memory.
 * Must be used after EEPROM.begin() and must use EEPROM.end()
 */
void clearSensorEEPROM() {
  Serial.println("Clearing EEPROM memory.");
  for (int i = 0; i < 1024; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("Cleared");
}

