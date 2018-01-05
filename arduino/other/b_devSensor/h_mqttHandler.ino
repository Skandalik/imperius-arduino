String registeredTopic ="";

String setStatusTopic ="";
String allCheckStatusTopic ="";
String checkStatusResponseTopic= "";

void setupMqtt() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  registeredTopic += "sensor/";
  registeredTopic += sensorIdToEeprom;
  registeredTopic += "/registered";
  
  checkStatusResponseTopic += "sensor/";
  checkStatusResponseTopic += sensorIdToEeprom;
  checkStatusResponseTopic += "/status/response";

  allCheckStatusTopic += "sensor/all/status/check";
  
  setStatusTopic += "sensor/";
  setStatusTopic += sensorIdToEeprom;
  setStatusTopic += "/status/set";
}

void callback(String topic, byte* message, unsigned int length) {
  String messageTemp;
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print(". Message: ");

  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  
  Serial.println(messageTemp);
  Serial.println();
  
  JsonObject& sensor = jsonBuffer.createObject();
  char jsonChar[100] = "";

  if (topic == registeredTopic.c_str() ) {
    registeredFlag = true;
    client.unsubscribe(registeredTopic.c_str());
    Serial.println("Sensor has been registered");
  }

  
  if (topic == allCheckStatusTopic.c_str() ) {
    int currentStatus = digitalRead(ledPin);
    sensor["uuid"] = sensorId.c_str();
    sensor["status"] = currentStatus;
    sensor["action"] = "update";
    sensor.printTo((char*)jsonChar, sensor.measureLength() + 1);
    client.publish("sensor/response", jsonChar); 
    jsonBuffer.clear();
  }
  
  
  if (topic == setStatusTopic.c_str() ) {
    if (messageTemp == "1") {
      digitalWrite(ledPin, HIGH);
      Serial.print("ON");
    }
    else if (messageTemp == "0") {
      digitalWrite(ledPin, LOW);
      Serial.print("Off");
    }
  }
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(sensorIdToEeprom)) {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      
      client.subscribe(registeredTopic.c_str());
      client.subscribe(setStatusTopic.c_str());
      client.subscribe(allCheckStatusTopic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
