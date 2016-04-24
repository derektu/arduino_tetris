/*
 * BT Test
 * 
 * 從bluetooth讀到'1'後, 點亮LED at pin 13, 讀到'0'後, 滅掉LED at pin 13
 */
#include <SoftwareSerial.h>

SoftwareSerial BT(10, 11);

const byte ledPIN = 13;

void setup() {
  Serial.begin(9600);
  pinMode(ledPIN, OUTPUT);
  BT.begin(115200);
  BT.println("BT is ready...");
  Serial.println("BT test started...");
}

void loop() {
  if (BT.available()) {
    char szLine[256];
    char val = BT.read();
    sprintf(szLine, "BT send:%c\n", val);
    Serial.print(szLine);
    switch(val) {
      case '0':
        digitalWrite(ledPIN, LOW);
        break;
      case '1':
        digitalWrite(ledPIN, HIGH);
        break;
    }
    BT.println(val); // 從BT echo回來
  }
  delay(1000);
  BT.print('.');
}
