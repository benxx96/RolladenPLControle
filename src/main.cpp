#include <Arduino.h>

#include <SoftwareSerial.h>

// Define
#define RX_PIN 10   // Virtual Reciever Pin
#define TX_PIN 11   // Virtual Transmitter Pin

// Variable
SoftwareSerial mySerial(RX_PIN, TX_PIN); // RX, TX

struct Data 
{
  unsigned int id = -1;
  String content = ""; 
};

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
}

void serialWrite(){
  // noInterrupts();
  Data data;
  data.id = 1;
  data.content = "test";

  if (mySerial.available()){
    // begin-marker
    mySerial.print("**");
    
    // content
    mySerial.print("id:");
    mySerial.print(data.id);
    mySerial.print(";");

    mySerial.print("content:");
    mySerial.print(data.content);

    // end-marker
    mySerial.print("$$");
  }
}

void serialRead(){
  String raw_data_str = "";
  String data_str = "";

  mySerial.listen();
  // while there is data coming in, read it
  // and send to the hardware serial port:
  if(mySerial.available() > 0){
    Serial.println("Data from mySerial:");
    while (mySerial.available() > 0) {
      char inByte = mySerial.read();
      // Serial.write(inByte);
      raw_data_str += inByte;
    }
    data_str = raw_data_str.substring(raw_data_str.indexOf('*')+2, raw_data_str.indexOf('$'));
    Serial.println(data_str);
    Serial.println();
  }
}

void loop() {
  // serialWrite();
  serialRead();
  delay(800);
}