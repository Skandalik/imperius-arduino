#include <DHT.h>

#define DHT_PIN 2
#define DHT_TYPE DHT11


DHT dht(DHT_PIN, DHT_TYPE);
float temperature;
float humidity;



// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually chontrols the LEDs
void setup() {
  delay(1000);
  
  Serial.begin(115200);
  dht.begin();
}

// For this project, you don't need to change anything in the loop function.
// Basically it ensures that you ESP is connected to your broker
void loop() {
  delay(1000);
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("*C. Humidity: ");
  Serial.print(humidity);
  Serial.println(" pascals.");
}

