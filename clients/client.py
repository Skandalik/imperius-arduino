import paho.mqtt.client as mqtt
import uuid
import json
import random
import socket
import struct


def on_publish(client, userdata, mid):
    print("Broadcasting as new.")

def on_message(client, userdata, message):
    print("Received message = ",str(message.payload.decode("utf-8")))

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribing to two topics.")

broker_addr = "localhost"

# sensorId = str(uuid.uuid4()).upper()
sensorId = 'bbb'
switchable = 1
status = 1
sensorIp = socket.inet_ntoa(struct.pack('>I', random.randint(1, 0xffffffff)))

client =mqtt.Client(sensorId)
client.on_message=on_message
client.on_publish=on_publish

client.connect(broker_addr)

data = {
    'uuid'   : sensorId,
    'switchable' : switchable,
    'value'      : status,
    'ip'   : sensorIp
}

client.publish("register", json.dumps(data), 1)

client.subscribe(sensorId + "/registered")
client.subscribe(sensorId)

client.loop_forever()