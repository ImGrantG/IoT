import paho.mqtt.client as mqtt
import pymysql

# Define the MQTT broker and topic
BROKER_URL = "broker.hivemq.com"  # Public HiveMQ broker
BROKER_PORT = 1883               # MQTT port
TOPIC = "testtopic/temp/room1"   # Topic to subscribe to

# Define MySQL database connection details
DB_HOST = "193.203.166.209"    # Replace with the actual MySQL host
DB_USER = "u255650614_imgrant"  # Your MySQL username
DB_PASSWORD = "9>oE6ril2@Ri"    # Your MySQL password
DB_NAME = "u255650614_mqtt"     # Your database name

# Establish a connection to the MySQL database
try:
    db_conn = pymysql.connect(
        host=DB_HOST,
        user=DB_USER,
        password=DB_PASSWORD,
        database=DB_NAME
    )
    cursor = db_conn.cursor()
    print("Connected to the MySQL database.")
except pymysql.MySQLError as err:
    print(f"Error connecting to MySQL database: {err}")
    raise

# Ensure the table exists in the database
TABLE_CREATION_QUERY = """
CREATE TABLE IF NOT EXISTS mqtt_messages (
    id INT AUTO_INCREMENT PRIMARY KEY,
    topic VARCHAR(255) NOT NULL,
    message TEXT NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)
"""
cursor.execute(TABLE_CREATION_QUERY)
db_conn.commit()

# Define callback for connection
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to HiveMQ broker!")
        # Subscribe to the topic once connected
        client.subscribe(TOPIC)
        print(f"Subscribed to topic: {TOPIC}")
    else:
        print(f"Failed to connect, return code {rc}")

# Define callback for receiving messages
def on_message(client, userdata, msg):
    message = msg.payload.decode()
    topic = msg.topic
    print(f"Received message: {message} from topic: {topic}")
    
    # Insert the received message into the MySQL database
    try:
        cursor.execute(
            "INSERT INTO mqtt_messages (topic, message) VALUES (%s, %s)",
            (topic, message)
        )
        db_conn.commit()
        print("Message saved to database.")
    except pymysql.MySQLError as err:
        print(f"Error saving message to database: {err}")

# Create an MQTT client instance
client = mqtt.Client()  # Use default MQTT transport

# Attach callback functions
client.on_connect = on_connect
client.on_message = on_message

# Connect to the HiveMQ broker
print("Connecting to broker...")
client.connect(BROKER_URL, BROKER_PORT, 60)

# Start the MQTT client loop
try:
    client.loop_forever()  # Keep the connection alive
except KeyboardInterrupt:
    print("Disconnecting...")
    client.disconnect()
    cursor.close()
    db_conn.close()
    print("Disconnected from the database.")
