
// "Bling" up your Staircase By Simon Jowett November 2014
// Thanks to the Neopxel Library by Adafruit


#include <Adafruit_NeoPixel.h>

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(117, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// Set up Variables
 unsigned long timeOut=60000; // timestamp to remember when the PIR was triggered.
 int downUp = 0;              // variable to rememer the direction of travel up or down the stairs
 int alarmPinTop = 10;        // PIR at the top of the stairs
 int alarmPinBottom =11;      // PIR at the bottom of the stairs
 int alarmValueTop = HIGH;    // Variable to hold the PIR status
 int alarmValueBottom = HIGH; // Variable to hold the PIR status
 int ledPin = 13;           // LED on the arduino board flashes when PIR activated
 int LDRSensor = A0;        // Light dependant resistor
 int LDRValue = 0;          // Variable to hold the LDR value
 int colourArray[350];      // An array to hold RGB values
 int change = 1;            // used in 'breathing' the LED's
 int breathe = 0;           // used in 'breathing' the LED's
 

void setup() {
   strip.begin();
   strip.show(); // Initialize all pixels to 'off'
   Serial.begin (9600);  // only requred for debugging
   pinMode(ledPin, OUTPUT);  // initilise the onboard pin 13 LED as an indicator
   pinMode(alarmPinTop, INPUT_PULLUP);     // for PIR at top of stairs initialise the input pin and use the internal restistor
   pinMode(alarmPinBottom, INPUT_PULLUP);  // for PIR at bottom of stairs initialise the input pin and use the internal restistor
   delay (2000); // it takes the sensor 2 seconds to scan the area around it before it can 
   //detect infrared presence.
   for (int i=0 ;i < 350; i++)  { // initilise the colourArray to zero
   colourArray[i]=0; 
   } 
}

void loop() {
  
  LDRValue = analogRead(LDRSensor);
  Serial.println(LDRValue);
  
  if (timeOut+57000 < millis()) {        // idle state - 'breathe' the top and bottom LED to show program is looping
    
     breathe = breathe + change;
     strip.setPixelColor(0,0,0,breathe);
     strip.setPixelColor(116,0,0,breathe);
     strip.show();
     if (breathe == 100 || breathe == 0) change = -change;      // breathe the LED from 0 = off to 100 = fairly bright
     if (breathe == 100 || breathe == 0) delay (300);           // Pause at beginning and end of each breath
     delay(25); 
  }
  
  if (LDRValue > 600) {        // only switch on LED's at night when LDR senses low light conditions - you may have to change the number for your circumstances!
  
    alarmValueTop = digitalRead(alarmPinTop);        // Constantly poll the PIR at the top of the stairs
    alarmValueBottom = digitalRead(alarmPinBottom);  // Constantly poll the PIR at the bottom of the stairs
    
    if (alarmValueTop == LOW && downUp != 2)  {      // the 2nd term allows timeOut to be contantly reset if one lingers at the top of the stairs before decending but will not allow the bottom PIR to reset timeOut as you decend past it.
      timeOut=millis();  // Timestamp when the PIR is triggered.  The LED cycle wil then start.
      downUp = 1;
      topdown();         // lights up the strip from top down
    }
 
    if (alarmValueBottom == LOW && downUp != 1)  {    // the 2nd term allows timeOut to be contantly reset if one lingers at the bottom of the stairs before decending but will not allow the top PIR to reset timeOut as you decend past it.
      timeOut=millis();    // Timestamp when the PIR is triggered.  The LED cycle wil then start.
      downUp = 2;
      bottomup();         // lights up the strip from bottom up
    }
    if (timeOut+10000 < millis() && timeOut+15000 < millis()) {    //switch off LED's in the direction of travel.
       if (downUp == 1) {
          colourWipeDown(strip.Color(0, 0, 0), 50); // Off
       }
       if (downUp == 2)  {
        colourWipeUp(strip.Color(0, 0, 0), 50);   // Off
       }
      downUp = 0;
      //   for (int i=0 ;i < 350; i++)  {          // Depending on your preference you may want to include this loop to clear out the colourArray
      //    colourArray[i]=0; 
      // }
    }
    
    if (timeOut+15000 < millis() && timeOut+54999 > millis()) waterfall();    // Waterfall effect to play between these times after a PIR trigger.
        
    if (timeOut+55000 < millis() && timeOut+56999 > millis()) fade();   // Fade/switch off LED's
  }
}

 void topdown() {
    Serial.println ("detected top");                // Helpful debug message
    colourWipeDown(strip.Color(50, 50, 30), 40);    // Warm White
    for(int i=0; i<3; i++) {                        // Helpful debug indication flashes led on Arduino board twice
      digitalWrite(ledPin,HIGH);
      delay(200);
      digitalWrite(ledPin,LOW);
      delay(200);
    }
 }

 void bottomup() {
    Serial.println ("detected bottom");          // Helpful debug message
    colourWipeUp(strip.Color(50, 50, 30), 40);   // Warm White
    for(int i=0; i<3; i++) {                     // Helpful debug indication flashes led on Arduino board twice
      digitalWrite(ledPin,HIGH);
      delay(200);
      digitalWrite(ledPin,LOW);
      delay(200);
    }
  }

 // Fill the dots one after the other with a color
 void colourWipeDown(uint32_t c, uint8_t wait) {
   for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
 }

 // Fill the dots one after the other with a color
 void colourWipeUp(uint32_t c, uint8_t wait) {
   for(uint16_t i=strip.numPixels(); i < -1; i--) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
 }

 // Play the Waterfall effect
 void waterfall(){
    for(int i=347; i>-1; i--) {            // Shift the RGB colours down in the colourArray[] 
        colourArray[i+3]=colourArray[i];
        }
      colourArray[0] = random(0,20);        // Genearate the red component of the 1st LED at the top of the stairs
      colourArray[1] = random(0,40);        // Genearate the green component of the 1st LED at the top of the stairs
      colourArray[2] = random(10,70);       // Genearate the blue component of the 1st LED at the top of the stairs
    
      for (int k=0; k<351; k=k+3)   {       // Set and send the colours to the strip
       uint32_t c = strip.Color(colourArray[k],colourArray[k+1],colourArray[k+2]);
       strip.setPixelColor(((k+3)/3)-1,c);
       }
   
      strip.show();      // and display the result
      delay(35);         // delay to simulate running water 
 }
 
 // Play the LED fade out
 void fade(){
 for (int j = 0; j <70; j++) {
   for(int i=350; i>-1; i--) {
     colourArray[i]=colourArray[i]-1;      // reduce intensity of light by 1
     if (colourArray[i] <= 0 ) colourArray[i] = 0;
   }
   for (int k=0; k<351; k=k+3)   {
     uint32_t c = strip.Color(colourArray[k],colourArray[k+1],colourArray[k+2]);
     strip.setPixelColor(((k+3)/3)-1,c);
   }
  strip.show();
  delay(60);
  }
 breathe = 0;
 change = 1; 
 }

