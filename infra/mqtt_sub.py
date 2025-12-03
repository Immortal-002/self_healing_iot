# infra/mqtt_sub.py
import paho.mqtt.client as mqtt

BROKER = "localhost"
TOPIC = "test/selfheal"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        client.subscribe(TOPIC)
    else:
        print("Connect failed", rc)

def on_message(client, userdata, msg):
    print(msg.payload.decode())

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, 1883, 60)
client.loop_forever()
