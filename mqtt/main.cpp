#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi variables
const char* ssid = "buster";  // WiFi name
const char* password = "abcdef1234";  // WiFi password

// MQTT variables
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* publishTopic = "testtopic/temp/outTopic/grant";
const char* subscribeTopic = "testtopic/temp/inTopic/grant"; // Separate inTopic
const char* triggerTopic = "testtopic/temp/triggerTopic";    // Topic for GPIO trigger
#define publishTimeInterval 10000 // in milliseconds

// Definitions
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
#define BUILTIN_LED 2 // built-in LED
#define INPUT_PIN D2  // GPIO pin for input trigger
char msg[MSG_BUFFER_SIZE];
int value = 0;
bool triggerSent = false;
unsigned long triggerStartTime = 0;

// Analog input pin
#define ANALOG_PIN A0

WiFiClient espClient;
PubSubClient client(espClient); // Define MQTT client

void setup_wifi() { 
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Restarting...");
    ESP.restart();
  }
}

void callback(char* topic, byte* payload, int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Check if the message is from the subscribeTopic
  if (String(topic) == subscribeTopic) {
    // Parse payload and toggle the LED
    if (length > 0 && payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW); // Turn LED on
      Serial.println("LED turned ON based on subscribed message.");
    } else if (length > 0 && payload[0] == '0') {
      digitalWrite(BUILTIN_LED, HIGH); // Turn LED off
      Serial.println("LED turned OFF based on subscribed message.");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(subscribeTopic);
      Serial.print("Subscribed to topic: ");
      Serial.println(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH); // Ensure LED is off initially
  pinMode(INPUT_PIN, INPUT);

  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883); // Use port 1883 for non-secure MQTT
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();

  // Handle periodic voltage publishing
  if (now - lastMsg > publishTimeInterval) {
    lastMsg = now;

    // Read voltage from the analog pin
    int analogValue = analogRead(ANALOG_PIN);
    float voltage = analogValue * (3.3 / 1023.0); // Convert to voltage (assuming 3.3V reference)

    // Publish the voltage reading
    snprintf(msg, MSG_BUFFER_SIZE, "Voltage: %.2f V", voltage);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publishTopic, msg);
  }

  // Handle GPIO input trigger
  if (digitalRead(INPUT_PIN) == HIGH && !triggerSent) {
    snprintf(msg, MSG_BUFFER_SIZE, "1");
    Serial.println("GPIO trigger detected. Sending 1 to triggerTopic.");
    client.publish(triggerTopic, msg);
    triggerStartTime = now;
    triggerSent = true;
  }

  // Send 0 after 5 seconds if trigger was sent
  if (triggerSent && now - triggerStartTime >= 5000) {
    snprintf(msg, MSG_BUFFER_SIZE, "0");
    Serial.println("5 seconds elapsed. Sending 0 to triggerTopic.");
    client.publish(triggerTopic, msg);
    triggerSent = false;
  }
}

