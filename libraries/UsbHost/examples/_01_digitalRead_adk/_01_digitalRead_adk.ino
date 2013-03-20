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

// button variables
int buttonPin = A1;
int buttonState = 0;
char letter = 'a';

// counters
long timer = millis();


// initialize the accessory:
AndroidAccessory usb(companyName, accessoryName);

void setup() {
  // start the connection to the device over the USB host:
  usb.begin();

  pinMode(buttonPin, INPUT);   
}

void loop() {
  /* Read button state */
  buttonState = digitalRead(buttonPin);
  
  /* Print to usb */
  if(millis()-timer>100) { // sending 10 times per second
    if (usb.isConnected()) { // isConnected makes sure the USB connection is ope
      if (buttonState == HIGH) {
        usb.write( 'a' );
      }
      else{
        usb.write( ' ' );
      }
    }
    timer = millis();
  }
}





