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
int sliderPin = A1;
int val;

// counters
long timer = millis();


// initialize the accessory:
AndroidAccessory accessory(companyName, accessoryName);

void setup() {
  // start the connection to the device over the USB host:
  accessory.begin();

  pinMode(sliderPin, INPUT);   
}

void loop() {
  accessory.refresh();

  /* Read button state */
  val = analogRead(sliderPin);
  val /= 4;
  
  /* Print to usb */
  if(millis()-timer>100) { // sending 10 times per second
    if (accessory.isConnected()) { // isConnected makes sure the USB connection is ope
      accessory.write(val);
    }
    timer = millis();
  }
}







