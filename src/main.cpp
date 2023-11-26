#include <Arduino.h>

#include <SoftwareSerial.h>

// Define
#define RX_PIN 10   // Virtual Reciever Pin
#define TX_PIN 11   // Virtual Transmitter Pin
#define BUTTON_PIN 8
#define LED_PIN 9


// Variable
int bufferSize = 0;
byte status = 0;
bool sender = false;
bool sendData = false;
bool isSendingData = false;
bool hasReceivedData = false;
unsigned long receiveTimer = 0;
unsigned long receiveTimerLength = 200;
unsigned long sendTimer = 0;
unsigned long sendTimerLength = 10000;


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
  if(mySerial.available() > 1){Serial.print(inByte);}
  // Serial.print(inByte);
  if(inByte == '*'){
    int i=0;
    while(mySerial.available() < 7){
      delay(100);
      Serial.print(".");
      if(i>100){break;}
      i++;
    }
    for(int i=0; i<6; i++){
      inByte = (char)mySerial.read();
      if((int)inByte >= 0 && (int)inByte <= 127){
        data += inByte;
      }
      else{
        i--;
      }
      if(inByte == '$'){
        bufferSize = mySerial.available();
        while (mySerial.available() > 1)
        {
          mySerial.read();
        }
        Serial.println();
        Serial.print(bufferSize);
        Serial.print(" - reset buffer to: ");
        Serial.println(mySerial.available());
        break;
      }
    }
  }
  delay(50);
  return data;
}

void loop() {
  if(sender){
    if(digitalRead(BUTTON_PIN) == HIGH){
      sendData = true;
      isSendingData = true;
      hasReceivedData = false;
    }

    if(sendData && millis() > sendTimerLength + sendTimer){
      isSendingData = true;
      sendTimer = millis();
      // Serial.println("resend activate");
    }

    if(millis() > receiveTimerLength + receiveTimer){
      if(!isSendingData){
        String data;
        char code[] = "*0111$";
        data = serialRead();
        if (strcmp(data.c_str(), code) == 0 && !hasReceivedData){
          sendData = false;
          isSendingData = false;
          hasReceivedData = true;
          digitalWrite(LED_PIN, LOW);

          Serial.println(data);
        }
        // Serial.println("receiving");
      }
      else if(sendData && isSendingData && !hasReceivedData){
        delay(2000);
        digitalWrite(LED_PIN, HIGH);
        serialWrite('0', '1', '1', '0');
        isSendingData = false;
        Serial.println("sending");

        bufferSize = mySerial.available();
        while (mySerial.available() > 1)
        {
          mySerial.read();
        }
        Serial.println();
        Serial.print(bufferSize);
        Serial.print(" - reset buffer to: ");
        Serial.println(mySerial.available());
      }
      receiveTimer = millis();
    } 
  }
  else{
    if(millis() > receiveTimerLength + receiveTimer){
      String data;
      if(!isSendingData){
        char code[] = "*0110$";
        data = serialRead();
        if (strcmp(data.c_str(), code) == 0){
          hasReceivedData = true;
          isSendingData = true;

          Serial.println(data);
        }
      }
      else if(isSendingData){
        delay(2000);
        for(int i=0; i<5; i++){
          serialWrite('0', '1', '1', '1');
          isSendingData = false;
          Serial.println("sending");
        }
        if(status == HIGH){
          digitalWrite(LED_PIN, LOW);
          status = LOW;
        }
        else{
          digitalWrite(LED_PIN, HIGH);
          status = HIGH;
        }

        bufferSize = mySerial.available();
        while (mySerial.available() > 1)
        {
          mySerial.read();
        }
        Serial.println();
        Serial.print(bufferSize);
        Serial.print(" - reset buffer to: ");
        Serial.println(mySerial.available());
      }
      receiveTimer = millis();
    } 
  }
  delay(50);
}