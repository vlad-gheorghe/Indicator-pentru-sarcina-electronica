#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#define LCD_RST 10
#define LCD_CS  9
#define LCD_BL  8
#define LCD_DIN 11
#define LCD_CLK 13

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif


// Color definitions
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

uint8_t errorCode = 0;
#define __CS 10
#define __DC 8
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
Arduino's 8 bit: any
DUE: check arduino site
If you do not use reset, tie it to +3V3
*/


TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC, 9);

#if USESPI==1
#include <SPI.h>
#endif
Adafruit_ADS1115 ads1015;

//#include "term8x14_font.h"
//#include "seg7fill_12x23dig_font.h"
#include <Fonts/FreeSerif12pt7b.h>

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello!");
   //ads1015.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads1015.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
   ads1015.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads1015.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads1015.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads1015.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV)");
  ads1015.begin();

 tft.begin(); tft.fillScreen(); tft.setRotation(1);tft.setFont(&FreeSerif12pt7b);
  tft.setCursor(1, 40);tft.setTextColor(MAGENTA);
   tft.println("Electronic Load");
   tft.setTextColor(GREEN);
   tft.println("     @2021");
   tft.setTextColor(YELLOW);
   tft.println(" Vlad Gheorghe");
      delay(2000);
   tft.fillRect(0, 0 , 160, 128, BLACK);
 tft.setTextColor(GREEN);
 tft.setCursor(138, 38);
tft.setTextSize(1);
tft.drawRect(1, 1 , 158, 52, YELLOW);
tft.drawRect(1, 55 , 158, 52, YELLOW);
  tft.println("V");
  tft.setCursor(138, 91);tft.setTextColor(BLUE);
  tft.println("A");
   tft.setCursor(1, 124);tft.setTextColor(MAGENTA);
   tft.println("Electronic Load");
}

void loop(void)
{
  int16_t tensiune;
   int16_t curent;

  tensiune = ads1015.readADC_Differential_2_3();
  //Serial.print("Differential: "); Serial.print(tensiune); Serial.print("("); Serial.print(tensiune/47.58 * 0.3); Serial.println("V)");
  //int16_t  u=tensiune/47.58 * 0.3;

  tft.setCursor(2, 42);tft.setTextColor(GREEN);
  tft.fillRect(2, 5 , 132, 45, BLACK);
  tft.setTextSize(2);
  tft.print(tensiune/47.58 * 0.3);


  delay(300);

  curent = ads1015.readADC_Differential_0_1();
  // Serial.print("Differential: "); Serial.print(curent); Serial.print("("); Serial.print(curent/235.96 * 0.3); Serial.println("A)");

 tft.setCursor(2, 97);tft.setTextColor(BLUE);
  tft.fillRect(2, 59 , 132, 45, BLACK);
  tft.print(curent/235.96 * 0.3);
  delay(300);
}
