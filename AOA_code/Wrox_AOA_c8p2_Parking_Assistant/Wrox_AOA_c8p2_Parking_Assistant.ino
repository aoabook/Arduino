/*
 * Parking Assistant exaample
 * Professional Android Open Accessory Programming
 * chapter 8 - miniproject 2
 * 
 * Connects to a phone, and sends the value of the
 * distance measured by an ultrasound sensor.
 *
 * In this case Arduino should connect or be
 * connected to the phone, get a subscription
 * request and send publish packages to the
 * phone. The payload of those packages is one byte
 * containing the distance mapped as a value 0..255
 *
 * (c) 2012 Andreas Goransson & David Cuartielles
 * GPLv3
 */

#include <AndroidAccessory.h>
#include <P2PMQTT.h>

// timer
long timer = millis();

// are we subscribed?
boolean subscribed = false;

// store the sensor reading
int val = 0;

P2PMQTT mqtt(true); // add parameter true for debugging
P2PMQTTpublish pub;

void setup() {
  // use the serial port to monitor that things work
  Serial.begin(9600);
  Serial.println("ready");
  
  // initiate the communication to the phone
  mqtt.begin("Parking Assistant");
  mqtt.connect(0,60000);  // add 1min timeout
}

void loop() {
  // get a P2PMQTT package and extract the type
  int type = mqtt.getType(mqtt.buffer); 

  // depending on the package type do different things
  switch(type) {
    
    case SUBSCRIBE:
      subscribed = mqtt.checkTopic(mqtt.buffer, type, "us");
      if (subscribed) {
        Serial.println("subscribed");
      }
      break;

    default:
      // do nothing
      break;
  }

  // if we are connected and subscribed, then we can
  // send data
  if (mqtt.isConnected() && subscribed) {
    val = analogRead(A2);
    pub.fixedHeader = 48;
    pub.length = 5;
    pub.lengthTopicMSB = 0;
    pub.lengthTopicLSB = 2;
    pub.topic = (byte*) "us";
    pub.payload = (byte*) val;
    mqtt.publish(pub);
  }
}


