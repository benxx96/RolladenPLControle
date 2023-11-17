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

Data getDataOfString(String raw_data_str, String identifier = "**", int id = 1){
  Data data;

  String data_str = "";
  String data_str1 = "";
  String data_str2 = "";

  data_str = raw_data_str.substring(raw_data_str.indexOf(strstr(raw_data_str.c_str(), identifier.c_str()))+2, raw_data_str.indexOf(strstr(raw_data_str.c_str(), "$$")));
  data_str1 = data_str.substring(0, data_str.indexOf(';'));
  data_str2 = data_str.substring(data_str.indexOf(';')+1, data_str.length());

  int id_ist = data_str1.substring(data_str1.indexOf(':')+1).toInt();
  if(id_ist == id){
    data.id = id_ist;
    data.content = data_str2.substring(data_str2.indexOf(':')+1);
  }

  return data;
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
}

void serialWrite(int id){
  // noInterrupts();
  Data data;
  data.id = id;
  data.content = "data";

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

void serialRead(){
  String raw_data_str = "";
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
    data = getDataOfString(raw_data_str);

    if(data.id == 1){
      Serial.println("Data from mySerial:");
      Serial.println(data.id);
      Serial.println(data.content);
      Serial.println();
    }
  }
}

void loop() {
  // serialWrite(1);
  serialRead();
  delay(1000);
}