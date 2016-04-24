/*
 * 接收master傳送的顯示指令(via I2C)
 */
#include <Wire.h>                //wire library for I2C
#include <Colorduino.h>          //colorduino library

#define I2C_DEVICE_ADDRESS 0x08  //I2C address for this device (不同的arduino需要設定不同的數值) 

#define START_OF_DATA 0xFF       // data markers
#define END_OF_DATA 0xFE         // data markers
#define FRAME_SIZE 11            // 1 + 8 + 1 + 1              

// 指令內容: Wire.h內建的buffer size = 32, 不能超過這個大小
//
// <START_OF_DATA> <Row> <Pixel> <Pixel> .. <END_OF_DATA>
// 每一個指令包含 1 + 1 + 8 + 1 = 11 bytes
// 
// Colorduino座標軸
// 
//    <排線位置>
//       c0     c1     c2     c3     c4     c5     c6     c7
//    ----------------------------------------------------------
// r0 | <0,0>, <0,1>, <0,2>, <0,3>, <0,4>, <0,5>, <0,6>, <0,7> 
// r1 | <1,0>, <1,1>, <1,2>, <1,3>, <1,4>, <1,5>, <1,6>, <1,7>
//    | ..
// r7 | <7,0>, <7,1>, <7,2>, <7,3>, <7,4>, <7,5>, <7,6>, <7,7>
//    |
//
// 傳送時每一次傳送一個row的資訊, 先傳row的位置(0-7), 再傳這個row內每個Pixel的數值, Pixel的數值是0~7, 代表Palette的顏色位置
//
static byte buffer[8 * 8] = { 0 };          // 目前的display buffer

struct ColorRGB
{
  byte r;
  byte g;
  byte b;

  ColorRGB(byte r, byte g, byte b) { 
    this->r = r; 
    this->g = g; 
    this->b = b; 
  }
};

static ColorRGB Palette[8] = {
  ColorRGB(0, 0, 0),      // black,   empty
  ColorRGB(0, 255, 255),  // cyan,    I
  ColorRGB(0, 0, 255),    // blue,    J
  ColorRGB(210,105,30),     // orange,  L
  ColorRGB(255,255, 0),    // yellow,  O
  ColorRGB(0, 255, 0),    // green,   S
  ColorRGB(138,43,226),    // purple,  T
  ColorRGB(255, 0, 0),    // red,     Z
};

#define PALETTE_MAX (sizeof(Palette)/sizeof(Palette[0]))

void setup() {
  Colorduino.Init();
  // compensate for relative intensity differences in R/G/B brightness
  // array of 6-bit base values for RGB (0~63)
  // whiteBalVal[0]=red
  // whiteBalVal[1]=green
  // whiteBalVal[2]=blue
  unsigned char whiteBalVal[3] = {33,63,63}; // for LEDSEE 6x6cm round matrix
  Colorduino.SetWhiteBal(whiteBalVal);

  Wire.begin(I2C_DEVICE_ADDRESS); // join i2c bus as slave
  Wire.onReceive(receiveEvent);   // define the receive function for receiving data from master
}

static void display_frame() {
  byte r, c;
  for (r = 0; r < 8; r++) {
    for (c = 0; c < 8; c++) {
        byte v = buffer[r * 8 + c];
        Colorduino.SetPixel(r, c, Palette[v].r, Palette[v].g, Palette[v].b);
    }
  }
  Colorduino.FlipPage();
}

// TEST TEST
//
void loop_test() {
  if (Wire.available() < 3)
    return; 

  byte b_start = Wire.read();
  byte b_data = Wire.read();
  byte b_end = Wire.read();
  if (b_start == START_OF_DATA && b_end == END_OF_DATA) {
    int paletteIndex = b_data % 8;
    for (byte r = 0; r < 8; r++) {
      for (byte c = 0; c < 8; c++) {
        Colorduino.SetPixel(r, c, Palette[paletteIndex].r, Palette[paletteIndex].g, Palette[paletteIndex].b);
      }
    }
    Colorduino.FlipPage();    
  }
}

void ProcessRequest() {
  // wait until a complete frame is received
  //
  if (Wire.available() < FRAME_SIZE)
    return; 

  byte ch = Wire.read();
  if (ch == START_OF_DATA) {
    byte rowIndex = Wire.read();
    if (rowIndex < 8) {
      int idx = 0;
      byte row[8];  
      for (idx = 0; idx < 8; idx++) {
        byte value = Wire.read();
        if (value >= PALETTE_MAX)
          break;
        
        row[idx] = value;
      }
      if (idx == 8 && Wire.read() == END_OF_DATA) {
        for (byte c = 0; c < 8; c++) {
          buffer[rowIndex * 8 + c] = row[c];
        }
        display_frame();
        return;
      }
    }
  } 
  // Error: 想辦法跳到下一個frame
  while (Wire.available() > 0 && Wire.read() != END_OF_DATA) {}      
}

void loop() {
}

// 
void receiveEvent(int numBytes) {
  ProcessRequest();
}

