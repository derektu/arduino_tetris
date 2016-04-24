/*
 * 傳送顯示的指令給Colorduino
 */
#include <Wire.h>
#include <SoftwareSerial.h>
 
#define SLAVE_ADDRESS 0x08       //I2C address for the target slave

#define START_OF_DATA 0xFF       // data markers
#define END_OF_DATA 0xFE         // data markers
#define FRAME_SIZE 11            // 1 + 1 + 8 + 1              

// Frame的layout請參考 cduo_slave
//

// 目前的display buffer
//
static byte buffer[8 * 8] = { 0 };

static void setPixel(byte r, byte c, byte v) {
  buffer[r * 8 + c] = v;
}

static void fillBuffer(byte v) {
  int idx = 0;
  for (byte x = 0; x < 8; x++) {
    for (byte y = 0; y < 8; y++) {
      buffer[idx++] = v; 
    }
  }
}


static void showTest(byte address, byte x) {
  Wire.beginTransmission(address);
  Wire.write(START_OF_DATA);
  Wire.write(x);
  Wire.write(END_OF_DATA);  
  Wire.endTransmission();  
}


static void showAll(byte address) {
  for (int r = 0; r < 8; r++) {
      showRow(address, r);
  }
}

static void showRow(byte address, int rowIndex) {
  Wire.beginTransmission(address);
  Wire.write(START_OF_DATA);
  Wire.write(rowIndex);
  Wire.write(&buffer[rowIndex * 8], 8);
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
    fillBuffer(0);
    showAll(SLAVE_ADDRESS);
  }
  else {
    for (byte c = 0; c < 8; c++) {
      setPixel(state, c, state);
      showRow(SLAVE_ADDRESS, state);
      delay(200);
    }
  }
  state++;
  if (state >= 8)
    state = 0;
}

void loop_xyz() {
  if (state == 0) {
    fillBuffer(6);

    for (byte r = 0; r < 8; r++) {
      showRow(SLAVE_ADDRESS, r);
      delay(500);
    }

    fillBuffer(0);
    showAll(SLAVE_ADDRESS);
  }
  /*
  else {
    byte r, c;
    fillBuffer(0);
    for (r = 0; r < state; r++) {
      for (c = 0; c < 8; c++) {
        setPixel(r, c, r);
      }    
    }
    show(SLAVE_ADDRESS);
    delay(200);

    byte cols = 1;
    while (cols <= 8) {
      fillBuffer(0);
      for (r = 0; r < state; r++) {
        for (c = 0; c < 8; c++) {
          setPixel(r, c, r);
        }    
      }
      for (c = 0; c < cols; c++) {
        setPixel(state, c, state);
      }    
      show(SLAVE_ADDRESS);
      delay(200);
      cols++;
    }
  }
  */
  /*
  state++;
  if (state >= 8)
    state = 0;
  */  
}

