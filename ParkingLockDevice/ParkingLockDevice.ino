#include <SoftwareSerial.h>
#include "SNIPE.h"

String parkingLotId = "1";

#define TXpin 11
#define RXpin 10
#define ATSerial Serial
#define control1 3
#define control2 4
#define button1 6
#define button2 7
#define LSW1 8
#define LSW2 9
bool state = HIGH;

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";  

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

void setup() {
  ATSerial.begin(115200);
  
  pinMode(control1,OUTPUT);
  pinMode(control2,OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(LSW1, INPUT_PULLUP);
  pinMode(LSW2, INPUT_PULLUP);

  // put your setup code here, to run once:
  while(ATSerial.read()>= 0) {}
  while(!ATSerial);

  DebugSerial.begin(115200);

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
  // 시작 상황
  if (SNIPE.lora_send(parkingLotId+",start")){

    String used = SNIPE.lora_recv();
    DebugSerial.println(used);
  
    if (used == "true"){
      DebugSerial.println("Someone want to use device");
      DebugSerial.println("파킹락 눕히기");
      digitalWrite(control1,LOW);
      digitalWrite(control2,HIGH);
      state = LOW;
      if( state == LOW && digitalRead(LSW2) == LOW){
        DebugSerial.println("리미트 스위치2 작동");
        digitalWrite(control1,LOW);
        digitalWrite(control2,LOW);
        state = HIGH;
      }
    }else{
      DebugSerial.println("Anyone want to use yet..");
    }
  }

  // 종료 상황
// if 센서로 출차 확인되었다면,
  SNIPE.lora_send(parkingLotId+",end");

  DebugSerial.println("파킹락 일으켜 세우기");
  digitalWrite(control1,HIGH);
  digitalWrite(control2,LOW);
  state = LOW;
  if( state == LOW && digitalRead(LSW1) == LOW){
    DebugSerial.println("리미트 스위치1 작동");
    digitalWrite(control1,LOW); 
    digitalWrite(control2,LOW);
    state = HIGH;
   }
  
  delay(1000);
}
