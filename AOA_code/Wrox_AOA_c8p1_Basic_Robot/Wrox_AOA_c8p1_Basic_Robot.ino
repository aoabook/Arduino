/*
 * Basic Robot exaample
 * Professional Android Open Accessory Programming
 * chapter 8 - miniproject 1
 * 
 * Connects to a phone, and gets commands to move
 * two servo motors. This example runs a state machine
 * that reads one of the following packages from a phone:
 *
 * - 0: stop
 * - 1: move forwards
 * - 2: turn right
 * - 3: move backwards
 * - 4: turn left
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
#include <Servo.h>

// motor limits
#define MOTOR_STOP  90
#define MOTOR_MAX   120
#define MOTOR_MIN   60
#define INCREMENT   5    // speed for changing speed

// create servo objects
Servo myservoL;  
Servo myservoR; 

// variables to store current and next state of servos
byte servoL_Next = MOTOR_STOP;
byte servoR_Next = MOTOR_STOP;
byte servoL = MOTOR_STOP;
byte servoR = MOTOR_STOP;

// timer
long timer = millis();

// to store the data
int payload = -1;

P2PMQTT mqtt(true); // add parameter true for debugging

void setup() {
  // use the serial port to monitor that things work
  Serial.begin(9600);
  Serial.println("ready");
  
  // initiate the communication to the phone
  mqtt.begin("Basic Robot");
  mqtt.connect(0,60000);  // add 1min timeout

  // attach servos to pins 8 and 9
  myservoL.attach(8); 
  myservoR.attach(9); 
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
      payload = mqtt.getPayload(mqtt.buffer, type)[0];
      Serial.print("command: ");
      Serial.println(payload);
      nextMotorState(payload);
      break;

    default:
      // do nothing
      break;
  }

  // if we are connected and we are getting data
  // we will keep on updating the motor's position
  // in a smooth fashion, but if there is no connection
  // we will stop the motors as a safety measure
  if (mqtt.isConnected()) {
    if( millis() - timer > 20 ) {

      if(servoL < servoL_Next) {
        servoL += INCREMENT;
        if(servoL > MOTOR_MAX) servoL = MOTOR_MAX;
      }

      if(servoR < servoR_Next) {
        servoR += INCREMENT;
        if(servoR > MOTOR_MAX) servoR = MOTOR_MAX;
      }

      if(servoL > servoL_Next) {
        servoL -= INCREMENT;
        if(servoL < MOTOR_MIN) servoL = MOTOR_MIN;
      }

      if(servoR > servoR_Next) {
        servoR -= INCREMENT;
        if(servoR < MOTOR_MIN) servoR = MOTOR_MIN;
      }

      myservoL.write(servoL);
      myservoR.write(servoR);
      timer = millis();
    }
  } else {
    // turn off the motors, we want no problem!!
    myservoL.write(MOTOR_STOP);
    myservoR.write(MOTOR_STOP);
  }
}

// decide for motors' next state based on the
// command arriving from the phone
void nextMotorState(int command) {
  switch (command) {
    // stop motors
    case 0:
      servoL_Next = MOTOR_STOP;
      servoR_Next = MOTOR_STOP;
      break;
    // move forwards
    case 1:
      servoL_Next = MOTOR_MAX;
      servoR_Next = MOTOR_MIN;
      break;
    // turn right
    case 2:
      servoL_Next = MOTOR_MAX;
      servoR_Next = MOTOR_MAX;
      break;
    // move backwards
    case 3:
      servoL_Next = MOTOR_MIN;
      servoR_Next = MOTOR_MAX;
      break;
    // turn left
    case 4:
      servoL_Next = MOTOR_MIN;
      servoR_Next = MOTOR_MIN;
      break;
    default:
      break;
  }
}

