import paho.mqtt.client as mqtt
import mysql.connector

# MQTT settings
mqtt_broker = "26153c839e864d51b805827602b1df8e.s1.eu.hivemq.cloud"
mqtt_port = 8883
mqtt_user = "alrochman23"
mqtt_password = "Malivice$23"
mqtt_topic_esp1_sensor = "home/potentiometer/esp1"  # Topic for ESP1 potentiometer
mqtt_topic_esp2_sensor = "home/potentiometer/esp2"  # Topic for ESP2 potentiometer

# MySQL database settings
db_host = "localhost"
db_user = "u525695601_db_TonyRochman"
db_password = "Malivice$23$"
db_name = "u525695601_TonyRochman"

# Connect to MySQL database
db = mysql.connector.connect(
    host=db_host,
    user=db_user,
    password=db_password,
    database=db_name
)

cursor = db.cursor()

# MQTT callback function when a message is received
def on_message(client, userdata, msg):
    try:
        # Decode the message payload and convert to float
        sensor_value = float(msg.payload.decode())  # Convert string to float

        # Print the received sensor value
        print(f"Received potentiometer value from {msg.topic}: {sensor_value}")

        # Insert the sensor value into the appropriate table with automatic timestamp
        if msg.topic == mqtt_topic_esp1_sensor:
            cursor.execute("INSERT INTO esp1_sensor_data (sensor_value) VALUES (%s)", (sensor_value,))
            db.commit()
            print(f"Successfully sent ESP1 sensor value: {sensor_value} to database.")
        elif msg.topic == mqtt_topic_esp2_sensor:
            cursor.execute("INSERT INTO esp2_sensor_data (sensor_value) VALUES (%s)", (sensor_value,))
            db.commit()
            print(f"Successfully sent ESP2 sensor value: {sensor_value} to database.")

    except Exception as e:
        # If an error occurs, display the error message
        print(f"Error while processing message: {e}")

# Setup MQTT client
client = mqtt.Client()
client.username_pw_set(mqtt_user, mqtt_password)

# Assign the callback function for message handling
client.on_message = on_message

# Connect to the MQTT broker
client.tls_set()  # Enable TLS encryption
client.connect(mqtt_broker, mqtt_port)

# Subscribe to the MQTT topics for ESP1 and ESP2
client.subscribe(mqtt_topic_esp1_sensor)
client.subscribe(mqtt_topic_esp2_sensor)

# Start the MQTT loop to handle incoming messages
print("Subscribed to MQTT topics, waiting for messages...")
client.loop_forever()
