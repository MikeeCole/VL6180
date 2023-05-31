#include <Arduino.h>
#include <Wire.h>

/*
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#define PIN_WIRE_SDA        (6u)
//#define PIN_WIRE_SCL        (7u)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3C 
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32);
*/

#include <VL6180X_WE.h>
#define VL6180X_ADDRESS 0x29
VL6180xIdentification identification;
VL6180x sensor(VL6180X_ADDRESS);

// https://makeabilitylab.github.io/physcomp/advancedio/smoothing-input.html

const int numReadings = 10;
int readings;
float average = 0.0;            // the average

float _ewmaAlpha = 0.25;  // the EWMA alpha value (α)
double _ewma = 0;        // the EWMA result (Si), initialized to zero
float avgVal;
float distance;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  Serial.println("RP2040 ok");

  Wire.begin(); //Start I2C library
  delay(100); // delay .1s

  /*
   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

    // Clear the buffer
    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(40, 30);
    display.println("HOWDY !");
    display.display();

*/
  if(sensor.VL6180xInit() != 0){
    Serial.println("FAILED TO INITALIZE"); //Initialize device and check for errors
    }
  sensor.VL6180xDefautSettings(); //Load default settings to get started.
  delay(1000); // delay 1s

  _ewma = sensor.getDistance();
}

float avg() {
  float total = 0;              // the running total  
  int readIndex = 0;          // the index of the current reading

  while (readIndex < numReadings) {  
    distance = sensor.getDistance();

    Serial.print("raw ");
    Serial.print(readIndex);
    Serial.print(": ");
    Serial.print(distance);
    Serial.println();

    total = total + distance;
    readIndex = readIndex + 1;
    }

  Serial.print("  total: ");
  Serial.print(total);
  Serial.println();

  // calculate the average:
  average = total / numReadings;

  Serial.print(">avg: ");
  Serial.println(average);

  return average;
}

void loop() {
  avgVal = avg();
  // Apply the EWMA formula 
  _ewma = (_ewmaAlpha * avgVal) + (1 - _ewmaAlpha) * _ewma;
  
  Serial.print(">ewma:");  
  Serial.println(_ewma);  
}

