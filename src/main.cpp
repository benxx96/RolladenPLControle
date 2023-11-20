#include <Arduino.h>

#include <SoftwareSerial.h>

// Define
#define RX_PIN 10   // Virtual Reciever Pin
#define TX_PIN 11   // Virtual Transmitter Pin
#define BUTTON_PIN 8
#define LED_PIN 9


// Variable
bool sender = false;
bool isSendingData = false;
bool hasSendedData = false;
bool hasReceivedData = false;
unsigned long sendTimer = 0;
unsigned long sendTimerLength = 2000;


// Objects
SoftwareSerial mySerial(RX_PIN, TX_PIN); // RX, TX

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void serialWrite(char id, char id2, char action, char received){
  // Device, Device, Action, received
  delay(50);
  mySerial.print("**");
  mySerial.print(id);
  mySerial.print(id2);
  mySerial.print(action);
  mySerial.print(received);
  mySerial.print("$$");
  delay(50);
}

String serialRead(){
  String data = "";

  mySerial.flush();
  mySerial.listen();
  // while there is data coming in, read it
  // and send to the hardware serial port:
  char inByte = (char)mySerial.read();
  if(inByte == '*' && mySerial.available() > 6){
    for(int i=0; i<6; i++){
      inByte = (char)mySerial.read();
      if((int)inByte >= 0 && (int)inByte <= 127){
        data += inByte;
      }
      else{
        i--;
      }
      if(inByte == '$'){break;}
    }
  }
  delay(100);
  return data;
}

void loop() {
  if(sender){
    if(digitalRead(8) == HIGH){
      isSendingData = true;
      hasSendedData = false;
      hasReceivedData = false;
    }

    if(millis() > sendTimerLength + sendTimer){
      if(isSendingData && !hasSendedData && !hasReceivedData){
        digitalWrite(LED_PIN, HIGH);
        serialWrite('0', '1', '1', '0');
        isSendingData = false;
        hasSendedData = true;
        Serial.println("send");
      }
      else if(!isSendingData && hasSendedData && !hasReceivedData){
        String data;
        char code[] = "*0111$";
        data = serialRead();
        if (strcmp(data.c_str(), code) == 0){
          hasSendedData = false;
          hasReceivedData = true;
          digitalWrite(LED_PIN, LOW);
        }
        else{
          isSendingData = true;
          hasSendedData = false;
          hasReceivedData = false;
        }
      }
      sendTimer = millis();
    } 
  }
  else{
    if(millis() > sendTimerLength + sendTimer){
      String data;

      if(!hasReceivedData && !isSendingData){
        char code[] = "*0110$";
        data = serialRead();
        if (strcmp(data.c_str(), code) == 0){
          hasReceivedData = true;
          isSendingData = true;

          Serial.println(data);
        }
      }
      else if(hasReceivedData && isSendingData){
        serialWrite('0', '1', '1', '1');
        hasReceivedData = false;
        isSendingData = false;
      }
      sendTimer = millis();
    } 
  }
  delay(50);
}