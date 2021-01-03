#include <SoftwareSerial.h>
#include <Servo.h>
SoftwareSerial mySerial(2, 3);
Servo myservo;
#define PIN_13 13
#define PIN_12 12

#define RELAY_1 A2  //with opacity
#define RELAY_2 A3
#define RELAY_3 A4
#define RELAY_4 A5




String commands[9];
//cmd_esp0_device_on_4_op_50_relay1
void setup() {
    myservo.attach(9);
    pinMode(PIN_12, OUTPUT);
    Serial.begin(9600); 
    mySerial.begin(9600);
    setServo(180);
    mySerial.println("controller started @esp0");
    Serial.println("controller started @esp0");
}


void loop() {

  if (mySerial.available() > 0) {
    String incomingByte = mySerial.readString();
    mySerial.println(incomingByte);
    split(incomingByte);
    for(int i=0;i<6;i++){
      mySerial.println(commands[i]);
     }
    performAction(commands[4].toInt(),commands[3]);
  }
  if (Serial.available() > 0) {
    String incomingByte = Serial.readString();
    Serial.println("from esp0->"+incomingByte);
    if(incomingByte.startsWith("cmd_esp0")){
      
      split(incomingByte);
      for(int i=0;i<8;i++){
        Serial.println(commands[i]);
        delay(50);
      }
      performAction(commands[4].toInt(),commands[3]);
      setServo(commands[6].toInt());
    }else {
      Serial.println("<----wrong command recived---->");
      Serial.println(incomingByte);
    }
  }                 
}
void split(String str){
  int index=0;
  int count=0;
  while(str.indexOf("_")>0){
    index=str.indexOf("_");
    String wrd=str.substring(0,index);
    commands[count]=wrd;
    count++;
    str=str.substring(index+1,str.length());
  }
  commands[count]=str;
}
void performAction(int pin,String action){
  mySerial.println("perfoming action->"+action +" on pin:");
  delay(50);
  mySerial.println(pin);
  delay(50);
  switch(pin){
    case 1:
      if(action=="on")
          pinMode(RELAY_1, OUTPUT);
      else
         pinMode(RELAY_1, INPUT);   
      break;
    case 2:
      if(action=="on")
          pinMode(RELAY_2, OUTPUT);
      else
         pinMode(RELAY_2, INPUT);   
      break;
    case 3:
      if(action=="on")
          pinMode(RELAY_3, OUTPUT);
      else
         pinMode(RELAY_3, INPUT);  
      break;
    case 4:
      if(action=="on")
          pinMode(RELAY_4, OUTPUT);
      else
         pinMode(RELAY_4, INPUT);  
      break;
  }
}


void setServo(int angle){
  mySerial.println("setting opacity");
  delay(50);
  mySerial.println(angle);
  delay(50);
  myservo.write(angle);
}
