#include <Arduino.h>

#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// Define
#define RX_PIN 10   // Virtual Reciever Pin
#define TX_PIN 11   // Virtual Transmitter Pin
#define BUTTON_PIN 8
#define LED_PIN 9


#define DEVICE_ID 100

// Variable
bool sender = false;
bool isSendingData = false;
bool hasRecievedData = false;
unsigned int sendCounter = 0;

// Objects
SoftwareSerial mySerial(RX_PIN, TX_PIN); // RX, TX
StaticJsonDocument<256> jsonDoc;

struct Data {
  unsigned int id = -1;
  String content = ""; 
};

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void serialWrite(unsigned int id, String content){
  // noInterrupts();
  Data data;
  data.id = id;
  data.content = "data";

  // begin-marker
  mySerial.print("**");
  
  jsonDoc["id"] = id;
  jsonDoc["content"] = content;

  serializeJson(jsonDoc, mySerial);

  // end-marker
  mySerial.print("$$");
}

Data serialRead(unsigned int id){
  String raw_data_str = "";
  Data data;

  mySerial.listen();
  // while there is data coming in, read it
  // and send to the hardware serial port:
  if(mySerial.available() > 0){
    bool startMarker = false;
    bool stopMarker = false;

    char inByte = mySerial.read();
    Serial.print(inByte);
    if(inByte == '*'){
      delay(0);
      char inByte2 = mySerial.read();
      if(inByte2 == '*'){
        startMarker = true;
        delay(0);
      }
    }
    
    while(startMarker && !stopMarker && mySerial.available() > 0){
      char inByte = mySerial.read();
      if(inByte == '$'){
        delay(0);
        char inByte2 = mySerial.read();
        if(inByte2 == '$'){
          delay(0);
          stopMarker = true;
        }
      }
      else{
        raw_data_str += inByte;
        Serial.print(raw_data_str);
      }
    }
  }
  deserializeJson(jsonDoc, raw_data_str);
  data.id = jsonDoc["id"];
  String content = jsonDoc["content"];
  data.content = content;
  if(data.id == id && raw_data_str.length() > 3){
    return data;
  }
  else{
    data.id = 0;
    data.content = "";
    return data;
  }
}

void loop() {
  Data data;

  if(sender){
    if(digitalRead(8) == HIGH){
      isSendingData = true;
      sendCounter = 0;
    }

    if(isSendingData && sendCounter < 6){
      digitalWrite(LED_PIN, HIGH);
      serialWrite(1, "move");
      Serial.println("send Data");
      sendCounter++;
      
      delay(5000);
    }
    else{
      digitalWrite(LED_PIN, LOW);
    }
  }
  else{
    data = serialRead(1);

    if(data.id != 0){
      int switchCase = 0;
      if (data.content == "move"){switchCase = 1;}

      switch (switchCase)
      {
      case 1: // move
        Serial.println(data.id);
        Serial.println(data.content);
        Serial.println();
        break;
      
      default:
        Serial.println("no Data!");
        break;
      }
    }
    if(mySerial.available() > 15){
      Serial.print((char)mySerial.read());
    }
    delay(2000);
  }
}