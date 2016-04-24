/*
 * 傳送顯示的指令給Colorduino
 */
#include <Wire.h>
#include <SoftwareSerial.h>
 
#define SLAVE_ADDRESS_1 0x08     //I2C address for the target slave #1
#define SLAVE_ADDRESS_2 0x09     //I2C address for the target slave #2

#define START_OF_DATA 0xFF       // data markers
#define END_OF_DATA 0xFE         // data markers
#define FRAME_SIZE 11            // 1 + 1 + 8 + 1              

byte slaves[2] = { 0x08, 0x09 };

// Frame的layout請參考 cduo_slave
//

// 目前的display buffer (共兩份)
//
static byte buffer[8 * 8 * 2] = { 0 };

static void setPixel(byte slaveIndex, byte r, byte c, byte v) {
  buffer[slaveIndex * 64 + r * 8 + c] = v;
}

static void fillBuffer(byte slaveIndex, byte v) {
  int idx = 0;
  int startOffset = slaveIndex * 64;
  for (byte x = 0; x < 8; x++) {
    for (byte y = 0; y < 8; y++) {
      buffer[startOffset + idx++] = v; 
    }
  }
}

static void showAll(byte slaveIndex) {
  for (int r = 0; r < 8; r++) {
      showRow(slaveIndex, r);
  }
}

static void showRow(byte slaveIndex, int rowIndex) {
  Wire.beginTransmission(slaves[slaveIndex]);
  Wire.write(START_OF_DATA);
  Wire.write(rowIndex);
  byte* p = &buffer[slaveIndex * 64 + rowIndex * 8];
  Wire.write(p, 8);
  Wire.write(END_OF_DATA);  
  Wire.endTransmission();  
}

void setup() {
  Wire.begin(1);        // join i2c bus (address optional for master)  
  Serial.begin(9600);
}

static int state = 0; // 0 = black, 1 = line#1, 2 = line#2, 3 = line#, .. 

void loop() {
  if (state == 0) {
    fillBuffer(0, 0);
    fillBuffer(1, 0);
    showAll(0);
    showAll(1);
  }
  else {
    byte slave = (byte)(state / 8);
    int row = state % 8;
    for (byte c = 0; c < 8; c++) {
      setPixel(slave, row, c, row);
      showRow(slave, row);
      delay(200);
    }
  }
  state++;
  if (state >= 16)
    state = 0;
}


