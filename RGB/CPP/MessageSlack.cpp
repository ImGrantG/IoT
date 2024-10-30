#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>


const char* ssid = "buster"; 
const char* password = "abcdef1234"; 
      WiFiClientSecure host;
      HTTPClient client;

void testConnection();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
    delay(10);
  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print the local IP address
  Serial.println();
  Serial.println("Connected to WiFi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  testConnection();
  pinMode(D1, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(WiFi.status() == WL_CONNECTED)                     
  Serial.println("Connected");
  if(digitalRead(D1) == HIGH)
    {
      host.setInsecure();
      client.begin(host,"https://maker.ifttt.com/trigger/button_pressed/json/with/key/bE-hEDj8ikFl6P80Ubjhlm4FRsiwXWy9GyBziXxKuGk");
      client.addHeader("Content-Type","application/json");
      delay(1000);
      int httpsResponseCode = client.PUT("");
      if (httpsResponseCode > 0) {
        Serial.printf("HTTP Response code: %d\n", httpsResponseCode);
      } else {
        Serial.printf("Error sending PUT: %s\n", client.errorToString(httpsResponseCode).c_str());
      }
      delay(1000);
      }
  else
    Serial.println("Low");
  
}

void testConnection() {
  WiFiClient host;
  HTTPClient client;

  if (!client.begin(host,"https://maker.ifttt.com/trigger/button_pressed/json/with/key/bE-hEDj8ikFl6P80Ubjhlm4FRsiwXWy9GyBziXxKuGk")) {
    Serial.println("Connection failed!");
    return;
  }
  Serial.println("Connected to httpbin.org");
 host.stop();
}