/*
 This code reads the value from the load cell. 
 Use the other sketch to find the calibration factor first.
 
 This example is based on the code from SparkFun HX711 breakout board with a scale 
 by SparkFun Electronics: https://github.com/sparkfun/HX711-Load-Cell-Amplifier
 
 This example code uses bogde's library: https://github.com/bogde/HX711
 
 Arduino Uno setup:
 digital pin 2 -> HX711 CLK
 digital 3 -> DOUT
 5V -> VCC
 GND -> GND

 Adafruit Feather Huzzah ESP 8266 setup:
 SCL -> HX711 CLK
 SDA -> DOUT
 3V -> VCC
 GND -> GND
 
*/

#include "HX711.h"

#define calibration_factor -2007.0 //Use value from calibration sketch

// Use the following for Arduino Uno:
//#define DOUT  3
//#define CLK  2
// Use the following for Adafruit Huzzah: 
#define DOUT  4
#define CLK  5

HX711 scale(DOUT, CLK);

void setup() {
  // Use the following for Arduino:
  //Serial.begin(9600);
  // Use the following for Adafruit Huzzah:
  Serial.begin(115200);
  
  Serial.println("Scale demo");

  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();	//Assuming there is no weight on the scale at start up, reset the scale to 0

  Serial.println("Readings:");
}

void loop() {
  delay(1000);
  
  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1); //scale.get_units() returns a float
  Serial.print(" g"); //Use same units as those used when calibrating
  Serial.println();
}
