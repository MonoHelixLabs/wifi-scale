/*
 This code can be used to find the calibration factor.

 This example is based on the code from SparkFun HX711 breakout board with a scale 
 by SparkFun Electronics: https://github.com/sparkfun/HX711-Load-Cell-Amplifier
 
 This example code uses bogde's library: https://github.com/bogde/HX711

 Instructions from SparkFun:
 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
 Use this calibration_factor on the example sketch
 
 This example assumes grams (g). If you want to use something else, just change what is printed
 and of course find the right calibration factor.
 
 Arduino Uno setup:
 digital pin 2 -> HX711 CLK
 digital 3 -> DOUT
 5V -> VCC
 GND -> GND

 WemosD1mini setup:
 digital pin 2 -> HX711 CLK
 digital 3 -> DOUT
 3.3V -> VCC
 GND -> GND

 Adafruit Feather Huzzah ESP 8266 setup:
 SCL -> HX711 CLK
 SDA -> DOUT
 3V -> VCC
 GND -> GND

 A note on using WemosD1mini:
 We experienced some issues with the Wemos D1 Mini in that we got the reset message.
 Looking over others' work we saw that the issues could be related to Serial - and 
 seems to be the case. As we couldn't get this code to work with the Wemos we started
 commenting some of the Serial prints, adding delays etc. until it started working. 
 However, when we removed the comments (thus the original code) it still worked, so 
 we don't have a clear understanding of why it did not work before, just that the
 communication with the device is somewhat finicky.
 
*/

#include "HX711.h"

// Use the following for Arduino Uno:
//#define DOUT  3
//#define CLK  2
// Use the following for WemosD1mini:
#define DOUT D3
#define CLK D2
// Use the following for Adafruit Huzzah: 
//#define DOUT  4
//#define CLK  5

HX711 scale(DOUT, CLK);

// Example values:
// 1kg scale: -1902
// 2kg scale: -2007
// wemos: 2007

float calibration_factor = 2007;

void setup() {
  // Use the following for Arduino:
  //Serial.begin(9600);
  // Use the following for Adafruit Huzzah or WemosD1mini:
  Serial.begin(115200);
  Serial.println("Calibration starting...");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Send + or a to increase calibration factor");
  Serial.println("Send - or z to decrease calibration factor");

  scale.set_scale();
  scale.tare();	//Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {
  
  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1);
  Serial.print(" g"); //Change to whatever measurement makes sense
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  //Serial.println("looping...");
  
  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 1;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 1;
    //Serial.println(calibration_factor);
  }

  //delay(2000);
}
