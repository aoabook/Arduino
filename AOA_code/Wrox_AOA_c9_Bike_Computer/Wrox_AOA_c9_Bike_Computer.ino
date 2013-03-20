#include <AndroidAccessory.h>
#include <P2PMQTT.h>

#define PERIOD 30000  // seconds between data transfers over serial
#define RED_LED       9
#define RED_BUTTON    4
#define GREEN_LED     10
#define GREEN_BUTTON  5

// encode the command sent to the phone for the current topic
#define START    1
#define STOP     2
#define SEND     3

// button status
int greenButton = HIGH;
int greenButtonOld = HIGH;
int redButton = HIGH;
int redButtonOld = HIGH;

// are we subscribed?
boolean subscribed = false;

// are we recording?
boolean recording = false;

volatile int counterWheel = 0;
volatile int counterPedal = 0;

long timer = 0;  // time counter

P2PMQTT mqtt(); // add parameter true for debugging
P2PMQTTpublish pub;

void setup() {
  // init the Serial port
  Serial.begin(9600);
  Serial.println("ready");

  // make sure the interrupt pins have pull-ups active
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  // attach interrupts
  attachInterrupt(0, countWheel, FALLING); // wheel sensor on pin 2
  attachInterrupt(1, countPedal, FALLING); // pedal sensor on pin 3

  // initialize the timer
  timer = millis();

  // initiate the communication to the phone
  mqtt.begin("Bike Computer");
  mqtt.connect(0,60000);  // add 1min timeout

  // configure the pins for the LEDs and buttons
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
}

void loop() {
  if(!mqtt.isConnected()) {
    // no connection = no light
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
  } 
  else {
    // connection = Green LED on
    digitalWrite(GREEN_LED, HIGH);

    // get a P2PMQTT package and extract the type
    int type = mqtt.getType(mqtt.buffer); 

    // depending on the package type do different things
    switch(type) {
    case SUBSCRIBE:
      subscribed = mqtt.checkTopic(mqtt.buffer, type, "bc");
      if (subscribed) {
        Serial.println("subscribed");
      }
      break;
    default:
      // do nothing
      break;
    }

    // if we are connected and subscribed, then we can
    // send data periodically
    if (mqtt.isConnected() && subscribed) {
      greenButton = digitalRead(GREEN_BUTTON);
      redButton = digitalRead(GREEN_BUTTON);
      if (!greenButton && greenButtonOld) {
        recording = true;
        publishToPhone(START);
      }
      if (!redButton && redButtonOld) {
        recording = false;
        publishToPhone(STOP);
      }
      if(millis() - timer >= PERIOD && recording) {
        publishToPhone(SEND);
      }
      greenButtonOld = greenButton;
      redButtonOld = redButton;
    }
  }
}

// publish data back to the phone
void publishToPhone(byte control) {
  noInterrupts();  // disable interrupts
  pub.fixedHeader = 48;
  pub.length = 6;
  pub.lengthTopicMSB = 0;
  pub.lengthTopicLSB = 2;
  pub.topic = (byte*) "bc";
  pub.payload[0] = control & 0xFF;
  pub.payload[1] = counterWheel & 0xFF;
  pub.payload[2] = counterPedal & 0xFF;
  mqtt.publish(pub);
  Serial.print("Wheel: "); 
  Serial.println(counterWheel); 
  counterWheel = 0;
  Serial.print("Pedal: "); 
  Serial.print(counterPedal);
  counterPedal = 0;
  timer = millis();
  interrupts();  // enable interrupts
}

// declare the wheel's interrupt callback function
void countWheel() {
  counterWheel++;
}

// declare the pedal's interrupt callback function
void countPedal() {
  counterPedal++;
}


