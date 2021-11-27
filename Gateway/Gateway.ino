#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <WiFiClient.h>

WiFiClient wifiClient;

//const char* ssid = "jdhaun6F(2g)";
//const char* password = "jdhaun2468";

const char* ssid = "KT_GIGA_5G_WAVE2_13B7";
const char* password = "cg87ff5116";

void setup () {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    Serial.print("Connecting..");
  }

}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    int parkingLotId = 1;
    String url = "ec2-3-37-183-92.ap-northeast-2.compute.amazonaws.com:8080/api_v1/checking/" + String(parkingLotId);
    http.begin(wifiClient, url);
    int httpCode = http.GET();                   
    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println(payload);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }

    http.end();
  }

  delay(10000);
}
