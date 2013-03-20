/*
 * Kitchen Lamp Project
 * Professional Android Open Accessory Programming
 * chapter 10 - embedded code
 * 
 * Connects to a phone, and gets commands to control
 * a 114 LEDs long kitchen lamp. This example runs a state machine
 * that reads one of the following packages from a phone:
 *
 * - 0: VU meter, to show the amount of time left on a timer
 * - 1: SMS arrival
 * - 2: CALL arrival
 *
 * In this case Arduino should connect or be
 * connected to the phone, issue a subscription
 * request and receive publish packages from the
 * phone. The payload of those packages is one byte
 * containing the command and an eventual second byte
 * carrying a parameter.
 *
 * (c) 2012 Andreas Goransson & David Cuartielles
 * GPLv3
 */

#include <AndroidAccessory.h>
#include <P2PMQTT.h>
#include <HL1606strip.h>

#define STRIP_D 36
#define STRIP_C 38
#define STRIP_L 40
#define NUM_LEDS 114

#define VUMETER 0
#define SMS 1
#define CALL 2

HL1606strip strip = HL1606strip(STRIP_D, STRIP_L, STRIP_C, NUM_LEDS);

P2PMQTT mqtt(true);  // add true to see debug info over the serial port
boolean subscribed = false;
boolean firstTime = true;
long timer = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("ready");
  mqtt.begin("Kitchen Lamp");
}

void loop() {
  if(firstTime || (millis() - timer > 1000 && !mqtt.isConnected())) {
     lightsON(WHITE);
     timer = millis();
     firstTime = false;
  }

  int firstByteMSB = mqtt.getType(mqtt.buffer); 
  int payload = 0;

  switch(firstByteMSB) {
    case CONNECT:
      Serial.println("connected");
//XXX      if(!subscribed) subscribed = mqtt.subscribe("kl");
      break;

    case PUBLISH:
      payload = mqtt.getPayload(mqtt.buffer,PUBLISH)[0];
      executeCommand(payload);
      break;

    default:
      // do nothing
      break;
  }
}

void executeCommand(int c) {
  int val = 0;
  switch (c) {
    case VUMETER:
      // VU meter sends the value in the second byte
      // of the payload
      val = mqtt.getPayload(mqtt.buffer,PUBLISH)[1];
      vuMeter(val);
      break;

    case SMS:
      doubleDipping(WHITE);
      break;

    case CALL:
      knightRider(5);
      break;
    default:
      // do nothing
      break;
  }
}


// the parameter fixes the amount of LEDs that 
// will be moving back and forth
void knightRider(int width) {
  for (int i=0; i< strip.numLEDs() - width; i++) {
    for (int j = 0; j < width; j++) 
      strip.setLEDcolor(i+j, RED); 
    if (i != 0) 
      strip.setLEDcolor(i-1, BLACK);
    strip.writeStrip();     
  }
  for (int i=strip.numLEDs() - width; i > 0; i--) {
    for (int j = 0; j < width; j++) 
      strip.setLEDcolor(i+j, RED); 
    if (i != 0)
      strip.setLEDcolor(i+width, BLACK);
    strip.writeStrip();     
  }
}

// doubleDipping takes the color we will use to light up
// the LEDs
void doubleDipping(int color) {
  for (int i=0; i< int(strip.numLEDs()/2); i++) {
    strip.setLEDcolor(i, color); 
    strip.setLEDcolor(strip.numLEDs() - i, color); 
    strip.writeStrip();     
  }
}

// the parameter determines in percentage how many
// LEDs will be on
void vuMeter(int percent) {
  for (int i=0; i< int(strip.numLEDs()*percent/100); i++) {
    strip.setLEDcolor(i, BLUE); 
  }
  for (int i=int(strip.numLEDs()*percent/100); i < strip.numLEDs(); i++) {
    strip.setLEDcolor(i, BLACK); 
  }
  strip.writeStrip();     
}

// turn the all the LEDs on taking color as a parameter
void lightsON(int color) {
  for (uint8_t i=0; i < strip.numLEDs(); i++) {
    strip.setLEDcolor(i, color);
  }
  strip.writeStrip();   
}


