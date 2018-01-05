void setup() {
  // initialize GPIO_2 pin as an output pin.
  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(2, HIGH);   // turn the LED on
  delay(1000);
  digitalWrite(2, LOW);    // turn the LED off
  delay(1000);
}
