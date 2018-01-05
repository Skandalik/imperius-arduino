import paho.mqtt.client as mqtt
import uuid
import json
import random
import socket
import struct
from random import randint

SENSOR_TOPIC_PREFIX = "sensor/"

# BROKER_HOST = "192.168.1.102"
BROKER_HOST = "77.55.235.53"
# BROKER_HOST = "localhost"
REGISTER_NEW_TOPIC = "sensor/register"
REGISTERED = False

def on_publish(client, userdata, mid):
    print("Message: " + str(mid))

def on_message(client, userdata, message):
    topic = str(message.topic)
    print("Topic: " + topic)
    if topic == REGISTERED_TOPIC:
        REGISTERED = True
        client.unsubscribe(REGISTERED_TOPIC)
        print("Sensor has been registered.")
    elif topic in CHECK_STATUS_ARRAY:
        print("Check sensor")
        data = {
            'uuid'   : sensorId,
            'status'      : get_status(),
            'action' : 'update'
        }
        qos = 1
        client.publish(SENSOR_CHECK_RESPONSE_TOPIC, json.dumps(data), qos)
    elif topic == SET_STATUS_TOPIC:
        message_val = str(message.payload)
        print("Set value of sensor " + message_val)
        if message_val == 0:
            print("Turning off sensor")
        elif message_val == 1:
            print("Turning on sensor")
        set_status(message_val)
        data = {
            'uuid'   : sensorId,
            'status'      : get_status(),
            'action' : 'update'
        }
        qos = 1
        client.publish(SENSOR_CHECK_RESPONSE_TOPIC, json.dumps(data), qos)
    else:
        print("Not catchable topic")

def set_status(stat):
    global status
    status = stat

def get_status():
    return status

sensorId = str(uuid.uuid4()).upper()
fetchable = False
switchable = True
adjustable = False
status = 0
sensorIp = socket.inet_ntoa(struct.pack('>I', random.randint(1, 0xffffffff)))
REGISTERED_TOPIC = SENSOR_TOPIC_PREFIX + sensorId + "/registered"
SET_STATUS_TOPIC = SENSOR_TOPIC_PREFIX + sensorId + "/status/set"
CHECK_STATUS_TOPIC = SENSOR_TOPIC_PREFIX + sensorId + "/status/check"
CHECK_ALL_STATUS_TOPIC = SENSOR_TOPIC_PREFIX + "all/status/check"
CHECK_STATUS_ARRAY = [CHECK_STATUS_TOPIC, CHECK_ALL_STATUS_TOPIC]
SENSOR_CHECK_RESPONSE_TOPIC = SENSOR_TOPIC_PREFIX + sensorId + "/status/response"
SENSOR_LAST_WILL_TOPIC = SENSOR_TOPIC_PREFIX + "last-will"

broker_addr = BROKER_HOST

lastWillData = {
    'uuid' : sensorId,
    'action' : 'disconnect'
}
client = mqtt.Client(sensorId, clean_session=False, protocol=mqtt.MQTTv311)
client.on_message=on_message
client.on_publish=on_publish
client.will_set(SENSOR_LAST_WILL_TOPIC, json.dumps(lastWillData), 1, False)

client.connect(broker_addr)

client.subscribe(SET_STATUS_TOPIC)
client.subscribe(CHECK_ALL_STATUS_TOPIC)
client.subscribe(CHECK_STATUS_TOPIC)
client.subscribe(REGISTERED_TOPIC)
client.subscribe(sensorId)

if not REGISTERED:
    data = {
        'uuid'   : sensorId,
        'fetchable' : fetchable,
        'switchable' : switchable,
        'adjustable' : adjustable,
        'status'      : status,
        'ip'   : sensorIp,
        'action' : 'register'
    }
    qos = 1
    
    client.publish(REGISTER_NEW_TOPIC, json.dumps(data), qos)
    print("Published message: " + str(data) + " on topic " + REGISTER_NEW_TOPIC + " with QoS: " + str(qos))

client.loop_forever()