/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/home/kevin/Documents/GitHub/KeyboardFeatherwingInterruptDriven/src/KeyboardFeatherwingInterruptDriven.ino"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <Adafruit_GFX.h>
#include <BBQ10Keyboard.h>
#include <neopixel.h>

// #define STMPE_CS 6
// #define TFT_CS 9
// #define TFT_DC 10
// #define NEOPIXEL_PIN 11

// Particle Argon references
void KeyIsr(void);
void setup();
void loop();
#line 15 "/home/kevin/Documents/GitHub/KeyboardFeatherwingInterruptDriven/src/KeyboardFeatherwingInterruptDriven.ino"
#define TFT_CS D4
#define TFT_DC D5
#define STMPE_CS D3
#define NEOPIXEL_PIN D6

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

Adafruit_STMPE610 ts(STMPE_CS);
Adafruit_ILI9341 tft(TFT_CS, TFT_DC);
BBQ10Keyboard keyboard;
// Adafruit_NeoPixel pixels(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels(1, NEOPIXEL_PIN, WS2812B);

// const int interruptPin = 5;
const int interruptPin = D8;

volatile bool dataReady = false;

void KeyIsr(void)
{
  dataReady = true;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  tft.begin();
  ts.begin();

  pixels.begin();
  pixels.setBrightness(30);
  
  keyboard.begin();
  keyboard.setBacklight(0.5f);

  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  tft.print("Hello FeatherWing!\n");

  ts.writeRegister8(STMPE_GPIO_ALT_FUNCT, _BV(2));  // Set pin 2 to GPIO
  ts.writeRegister8(0x16, _BV(2)); // Pin 2 falling edge detection enable

  // ts.writeRegister8(STMPE_INT_EN, STMPE_INT_EN_TOUCHDET | STMPE_INT_EN_GPIO); // Interrupts for screen and GPIO
  ts.writeRegister8(STMPE_INT_EN, STMPE_INT_EN_GPIO); // Interrupts for GPIO only
  ts.writeRegister8(0x0C, 1 << 2); // Pin 2 GPIO interrupt enable
  
  pinMode(D8, INPUT_PULLDOWN);
  attachInterrupt(interruptPin, KeyIsr, RISING);
}


void loop() {

  if (dataReady) {
    tft.print("Interrupt received!\n");

    // reset all interrupts
    ts.writeRegister8(STMPE_INT_STA, 0xFF);

    // Clear the GPIO interrupt status register
    ts.writeRegister8(0x0D, 0xFF);  

    keyboard.clearInterruptStatus();
    dataReady = false;
  }

  // Print keys to screen
  if (keyboard.keyCount()) {
    const BBQ10Keyboard::KeyEvent key = keyboard.keyEvent();
    if (key.state == BBQ10Keyboard::StateRelease) {
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
      pixels.show(); 
    
      tft.print(key.key);
      
      pixels.clear();
      pixels.show(); 
    }
  }
}