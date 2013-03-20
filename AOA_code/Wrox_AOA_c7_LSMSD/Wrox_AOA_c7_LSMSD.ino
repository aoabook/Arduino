/*
 * Basic MQTT Template
 * Professional Android Open Accessory Programming
 * 
 * In this case Arduino should connect or be
 * connected to the phone, issue a subscription
 * request and receive publish packages from the
 * phone. The payload of those packages is one byte
 * containing the command.
 *
 * (c) 2012 Andreas Goransson & David Cuartielles
 * GPLv3
 */

#include <AndroidAccessory.h>
#include <P2PMQTT.h>
#include "Display.h"

#define NO_PHONE -1
#define NO_SMS 0
#define DISPLAY_SMS 1
#define MAX_SHOW_SMS 60000 // show SMS for 1m.

// timer
long timer = millis();

// to store the data
byte* payload;

P2PMQTT mqtt(true); // add parameter true for debugging

char currentSMS[160] = {0}; // character array to contain the SMS
int mode = NO_PHONE; // variable to store the mode
long timerShowSMS = 0; // count for how long we showed the SMS
boolean subscribed = false;

void setup() {
  // use the serial port to monitor that things work
  Serial.begin(9600);
  Serial.println("ready");
  
  // initiate the communication to the phone
  mqtt.begin("LSMSD");
  mqtt.connect(0,60000);  // add 1min timeout

  // initialize the display
  ht1632_setup(); 
  randomSeed(analogRead(0));

  cls();
}

void loop() {
  // get a P2PMQTT package and extract the type
  int type = mqtt.getType(mqtt.buffer); 

  // depending on the package type do different things
  switch(type) {
    
    case CONNECT:
      Serial.println("connected");
      break;

    case PUBLISH:
      mqtt.getPayload(mqtt.buffer,PUBLISH, payload);
      mode = DISPLAY_SMS;
      Serial.println((char*) payload);
      timerShowSMS = millis();
      break;

    default:
      // do nothing
      break;
  }

  // if we are connected and we are getting data
  if (mqtt.isConnected() && mode == NO_PHONE) {
    mode = NO_SMS;
    if( millis() - timer > 20 ) {
      // do whatever based on the timer
      timer = millis();
    }
  } 

  // block discriminating between modes
  //String txt;
  switch (mode) {
    case NO_PHONE:
//      cls();  
      showText(0,4,"NO PHONE",ORANGE);
//      delay(500);
      break;
    case NO_SMS:
//      cls();  
      showText(0,4,"NO SMS", GREEN);
      break;
    case DISPLAY_SMS:
//      if(millis() - timerShowSMS > MAX_SHOW_SMS) {
        for(int i=0; i < 10; i++) {
          currentSMS[i] = (char)payload[i];
        }
        scrolltextxcolor(4,currentSMS,ORANGE,30);
//      } else mode = NO_SMS;
      break;
  }
  delay(100);
  cls();
}

void showText(int x, int y, char str[], int color) {
  int m = strlen(str)+ 1;
  for (int i=0; i < m; i++) {
    ht1632_putchar(x+6*i, y, str[i], color);
  }
}

