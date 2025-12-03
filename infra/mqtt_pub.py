# infra/mqtt_pub.py
import time
import json
import paho.mqtt.client as mqtt
from datetime import datetime

BROKER = "localhost"
TOPIC = "test/selfheal"

client = mqtt.Client()
client.connect(BROKER, 1883, 60)

node_id = "node-mqtt-1"
i = 0
try:
    while True:
        payload = {
            "node_id": node_id,
            "timestamp": int(time.time()),
            "seq": i,
            "value": i % 100
        }
        client.publish(TOPIC, json.dumps(payload))
        print("published", payload)
        i += 1
        time.sleep(1)
except KeyboardInterrupt:
    print("publisher stopped")
    client.disconnect()
