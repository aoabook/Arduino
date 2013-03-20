#include <AndroidAccessory.h>
#include <P2PMQTT.h>

#define TIMEOUT 1000

P2PMQTT mqtt(true);  // add true to see debug info over the serial port
boolean subscribed = false;

long timer = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println(“ready”);
  mqtt.begin("CV Robot");
  timer = millis();
}
void loop() {
  if((millis() - timer > TIMEOUT && !mqtt.isConnected())) {
     Serial1.write(0);
     timer = millis();
  }

  int firstByteMSB = mqtt.getType(mqtt.buffer); 
  int payload = 0;

  switch(firstByteMSB) {
    case CONNECT:
      Serial.println("connected");
      if(!subscribed) subscribed = mqtt.subscribe("mw");
      break;

    case PUBLISH:
      payload = mqtt.getPayload(mqtt.buffer,PUBLISH)[0];
      Serial.println(payload);
      Serial1.write(payload);
      break;

    default:
      // do nothing
      break;
  }
}
