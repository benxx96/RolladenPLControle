#include <Arduino.h>

#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// Define
#define RX_PIN 10   // Virtual Reciever Pin
#define TX_PIN 11   // Virtual Transmitter Pin

// Variable
SoftwareSerial mySerial(RX_PIN, TX_PIN); // RX, TX
StaticJsonDocument<256> jsonDoc;
StaticJsonDocument<256> jsonDoc2;

struct Data 
{
  unsigned int id = -1;
  String content = ""; 
};

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
}

void serialWrite(int id, String content){
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

void serialRead(){
  String raw_data_str = "";
  String data_str = "";
  Data data;

  mySerial.listen();
  // while there is data coming in, read it
  // and send to the hardware serial port:
  if(mySerial.available() > 0){
    // Serial.println("Data from mySerial:");
    while (mySerial.available() > 0) {
      char inByte = mySerial.read();
      // Serial.write(inByte);
      raw_data_str += inByte;
    }

    data_str = raw_data_str.substring(raw_data_str.indexOf("**")+2, raw_data_str.indexOf("$$"));
    deserializeJson(jsonDoc, data_str);

    data.id = jsonDoc["id"];
    String content = jsonDoc["content"];
    data.content = content;

    if(data.id == 1){
      Serial.println("Data from mySerial:");
      Serial.println(data.id);
      Serial.println(data.content);
      Serial.println();
    }
  }
}

void loop() {
  // serialWrite(1, "move");
  serialRead();
  delay(1000);
}