#include <SoftwareSerial.h>
#include "SNIPE.h"

String parkingLotId = "1";
int trig = 13; //초음파센서 쏘기
int echo = 12; //초음파센서 받기
long distance;
long duration;

#define TXpin 11
#define RXpin 10
#define ATSerial Serial
#define control1 3
#define control2 4
#define button1 6
#define button2 7
#define LSW1 9
#define LSW2 8
bool state = HIGH;

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";  

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

void setup() {
  ATSerial.begin(115200);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
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

float getDistanceCM(){
  digitalWrite(echo,LOW);
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  duration = pulseIn(echo, HIGH);
  distance = (duration*340)/2/10000;
  return distance;
  }

float getStableDistance(){
  int CmSum = 0;
  for (int i = 0; i<8; i++){
    CmSum += getDistanceCM();
    }
    return CmSum/8;
  }

void loop() {
  
  // 시작 상황
  if (SNIPE.lora_send(parkingLotId+",start")){

    String used = SNIPE.lora_recv();
    DebugSerial.println(used);

    if( digitalRead(LSW1) == LOW){
    if (used == "true"){
      Serial.println("파킹락 눕히기");
      digitalWrite(control1,LOW);
      digitalWrite(control2,HIGH);
      state = LOW;
      delay(5000);
      if( state == LOW && digitalRead(LSW2) == LOW){
        Serial.println("리미트 스위치2 작동");
        digitalWrite(control1,LOW);
        digitalWrite(control2,LOW);
        state = HIGH;
        delay(5000);
        }
      }else{
        DebugSerial.println("Anyone want to use yet..");
        }
    }
  }

  // 종료 상황
  if( digitalRead(LSW2) == LOW){
    Serial.println(getStableDistance());
    
    if(getStableDistance()>25){
      SNIPE.lora_send(parkingLotId+",end");
  
      Serial.println("파킹락 일으켜 세우기");
      digitalWrite(control1,HIGH);
      digitalWrite(control2,LOW);
      state = LOW;
      delay(10000);
      if( state == LOW && digitalRead(LSW1) == LOW){
        Serial.println("리미트 스위치1 작동");
        digitalWrite(control1,LOW); 
        digitalWrite(control2,LOW);
        state = HIGH;
        }
    }
  }
  delay(300);
}
