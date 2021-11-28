#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
#include "SNIPE.h"

#define TXpin 11
#define RXpin 10
#define ATSerial Serial

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";  

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

WiFiClient wifiClient;

const char* ssid = "jdhaun6F(2g)";
const char* password = "jdhaun2468";

//const char* ssid = "seongkyu";
//const char* password = "38906081";

void setup () {

  ATSerial.begin(115200);

  DebugSerial.begin(115200);

  // Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    DebugSerial.print("Connecting..");
  }
  DebugSerial.print("Connected!");

  // put your setup code here, to run once:
  while(ATSerial.read()>= 0) {}
  while(!ATSerial);

  /* SNIPE LoRa Initialization */
  if (!SNIPE.lora_init()) {
    DebugSerial.println("SNIPE LoRa Initialization Fail!");
    while (1);
  }

  /* SNIPE LoRa Set Appkey */
  if (!SNIPE.lora_setAppKey(lora_app_key)) {
    DebugSerial.println("SNIPE LoRa app key value has not been changed");
  }
  
  /* SNIPE LoRa Set Frequency */
  if (!SNIPE.lora_setFreq(LORA_CH_1)) {
    DebugSerial.println("SNIPE LoRa Frequency value has not been changed");
  }

  /* SNIPE LoRa Set Spreading Factor */
  if (!SNIPE.lora_setSf(LORA_SF_7)) {
    DebugSerial.println("SNIPE LoRa Sf value has not been changed");
  }

  /* SNIPE LoRa Set Rx Timeout 
   * If you select LORA_SF_12, 
   * RX Timout use a value greater than 5000  
  */
  if (!SNIPE.lora_setRxtout(5000)) {
    DebugSerial.println("SNIPE LoRa Rx Timout value has not been changed");
  }  
    
  DebugSerial.println("SNIPE LoRa PingPong Test");

}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // payload = parkingLotId + start or end
    String payload = SNIPE.lora_recv();
    int idx = payload.indexOf(",");
    String parkingLotId = payload.substring(0,idx);
    String startOrEnd = payload.substring(idx+1,payload.length());

    if (startOrEnd == "start"){
      //String parkingLotId = "1";
      String url = "http://ec2-3-37-183-92.ap-northeast-2.compute.amazonaws.com:8080/api_v1/check?id=" + parkingLotId;
      http.begin(wifiClient, url);
      int httpCode = http.GET();                   
      if (httpCode == 200) {
        String used = http.getString();
        DebugSerial.println(used);
        SNIPE.lora_send(used);
      }else{
        String used = "false";
        DebugSerial.println(used);
        SNIPE.lora_send(used);
      }
    }

 // if payload의 잘린 부분이 end라면,
    if (startOrEnd == "end"){
      String url = "http://ec2-3-37-183-92.ap-northeast-2.compute.amazonaws.com:8080/api_v1/parking_record_end";
      http.begin(wifiClient, url);
      int httpCode = http.PATCH(parkingLotId);
      if (httpCode == 200) {
        String idAndEndTime = http.getString();
        DebugSerial.println(idAndEndTime);
      }else {
        DebugSerial.println("not updated");
      }
    }
    
    http.end();

  }

  delay(1000);
}
