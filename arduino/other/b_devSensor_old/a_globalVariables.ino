#define SENSOR_ID_SIZE 20
#define SENSOR_ID_FLAG_ADDRESS 100
#define SENSOR_ID_INIT_FLAG 1

int sensorEepromAddressPointer = 0;
byte sensorId[SENSOR_ID_SIZE];

WiFiClient wifiClient;
PubSubClient client(wifiClient);
