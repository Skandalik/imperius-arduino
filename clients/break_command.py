import paho.mqtt.client as mqtt
import uuid
import json
import random
import socket
import struct

# broker_addr = "192.168.1.102"
broker_addr = "localhost"

# sensorId = str(uuid.uuid4()).upper()
sensorId = 'bbb'
switchable = 1
status = 1
sensorIp = socket.inet_ntoa(struct.pack('>I', random.randint(1, 0xffffffff)))

client =mqtt.Client(sensorId)

client.connect(broker_addr)

data = {
    'action' : 'update'
}

client.publish("sensor/response", "some", 1)

client.loop_forever()