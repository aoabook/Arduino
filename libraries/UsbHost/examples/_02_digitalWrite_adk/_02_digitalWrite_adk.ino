/*
* ADK usb digitalRead
 *
 * TADA!
 *
 * (c) 2012 D. Cuartielles & A. Goransson
 * http://arduino.cc, http://1scale1.com
 *
 */

#include <AndroidAccessory.h>

// accessory descriptor. It's how Arduino identifies itself to Android
char accessoryName[] = "Mega_ADK"; // your Arduino board
char companyName[] = "Arduino SA";

// led variables
int ledPin = 10;

// counters
long timer = millis();

// initialize the accessory:
AndroidAccessory usb(companyName, accessoryName);

void setup() {
  Serial.begin( 9600 );
  // start the connection to the device over the USB host:
  usb.begin();

  pinMode(ledPin, OUTPUT);   
}

void loop() {
  /* Print to usb */
  if(millis()-timer>100) { // sending 10 times per second
    if (usb.isConnected()) { // isConnected makes sure the USB connection is ope
      char val = usb.read();
      Serial.print( val );
      if( val == 'a' )
        digitalWrite( ledPin, HIGH );
      else if( val == 'b' )
        digitalWrite( ledPin, LOW );
    }
    timer = millis();
  }
}









