#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>


const char* ssid = "buster"; 
const char* password = "abcdef1234"; 
      WiFiClientSecure host;
      HTTPClient client;

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
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(WiFi.status() == WL_CONNECTED) 
  {                
  if(digitalRead(D1) == HIGH)
    {
      host.setInsecure();
      client.begin(host,"https://grantgoodwin.me/results.txt");
      delay(1000);
      int httpsResponseCode = client.GET();
      if (httpsResponseCode > 0) {
        Serial.println("HTTP Response: ");
        Serial.println(client.getString());
        if(client.getString() == "on")
          digitalWrite(D8, HIGH);
        if(client.getString() == "off")
          digitalWrite(D8, LOW);
      } else {
        Serial.printf("Error sending GET: %s\n", client.errorToString(httpsResponseCode).c_str());
      }
      delay(1000);
      }
    if(digitalRead(D2) == HIGH)
    {
      host.setInsecure();
      client.begin(host,"https://grantgoodwin.me/RGB.txt");
      delay(1000);
      int httpsResponseCode = client.GET();
      if (httpsResponseCode > 0) {
        Serial.println("HTTP Response: ");
        Serial.println(client.getString());
        String nums = client.getString();
        int pos1 = nums.indexOf('\n');
        int pos2 = nums.indexOf('\n',pos1+1);
        
        int var1 = nums.substring(0, pos1).toInt();
        int var2 = nums.substring(pos1 + 1, pos2).toInt();
        int var3 = nums.substring(pos2 + 1).toInt();

        analogWrite(D5, var1);
        analogWrite(D6, var2);
        analogWrite(D7, var3);

      } else {
        Serial.printf("Error sending GET: %s\n", client.errorToString(httpsResponseCode).c_str());
      }
      delay(1000);

    }
  }
  
}