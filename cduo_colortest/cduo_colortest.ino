#include <Colorduino.h>          //colorduino library

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

void setup() {
  Colorduino.Init();
  // compensate for relative intensity differences in R/G/B brightness
  // array of 6-bit base values for RGB (0~63)
  // whiteBalVal[0]=red
  // whiteBalVal[1]=green
  // whiteBalVal[2]=blue
  unsigned char whiteBalVal[3] = {33,63,63}; // for LEDSEE 6x6cm round matrix
  Colorduino.SetWhiteBal(whiteBalVal);
}

static int state = 0; // 0 = black, 1 = line#1, 2 = line#2, 3 = line#, .. 

static void FillColors(byte r, byte g, byte b) {
  byte x, y;
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
        Colorduino.SetPixel(x, y, r, g, b);
    }
  }
}

void loop() {
  byte x, y;
  if (state == 0) {
    Colorduino.ColorFill(0, 0, 0);
    delay(500);
    Colorduino.ColorFill(255, 255, 255);
    delay(500);
    Colorduino.ColorFill(0, 0, 0);
    delay(500);
  }
  else {
    FillColors(0, 0, 0);
    for (y = 0; y < state; y++) {
      for (x = 0; x < 8; x++) {
        Colorduino.SetPixel(x, y, Palette[y].r, Palette[y].g, Palette[y].b);
      }    
    }
    Colorduino.FlipPage();
    delay(200);

    byte cols = 1;
    while (cols <= 8) {
      FillColors(0, 0, 0);
      for (y = 0; y < state; y++) {
        for (x = 0; x < 8; x++) {
          Colorduino.SetPixel(x, y, Palette[y].r, Palette[y].g, Palette[y].b);
        }    
      }
      for (x = 0; x < cols; x++) {
        Colorduino.SetPixel(x, state, Palette[y].r, Palette[y].g, Palette[y].b);
      }    
      Colorduino.FlipPage();
      delay(200);
      cols++;
    }
  }
  state++;
  if (state >= 8)
    state = 0;
}


