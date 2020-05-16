#include <AccelStepper.h>
#include <Wire.h>
#include <VL53L0X.h>

// motor

AccelStepper stepper(1, 6, 5); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
int direction = 1;

// switch

int switchPin = 18;
bool isSwitchPressed = false;

//

VL53L0X sensor;
int NEAR = 50;

// state

int state = 0;
int STATE_INIT = 1;
int READY = 2;

void setup()
{
  state = STATE_INIT;

  // serial

  Serial.begin(9600);

  // switch

  Wire.begin();
  pinMode(switchPin, INPUT_PULLUP);

  // sensor

  Wire.begin();

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }

  sensor.startContinuous();

  // 

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(1000);

  Serial.println(stepper.maxSpeed());
  Serial.println(stepper.speed());
}

void loop()
{

  // sensor

  int sensorValue = sensor.readRangeContinuousMillimeters();
  bool isNear = sensorValue < NEAR;

  // switch

  int switchValue = digitalRead(switchPin);
  isSwitchPressed = switchValue == 0;
//  Serial.println(isSwitchPressed);

  // state

  Serial.println(state);

  if (state == STATE_INIT) {
    onInit();
  } else if (state == READY) {
    //    stepper.setSpeed(100);
    //    stepper.setMaxSpeed(1000);
  }

  //  //
  //
  ////  if(state == NORMAL_STATE){
  ////    if (stepper.distanceToGo() == 0)
  ////    {
  ////      direction *= -1;
  ////      delay(random(10, 100));
  ////      int distance = 10; //random(10,20); //random(,100);
  ////      stepper.moveTo(distance * direction);
  ////      ////    stepper.setMaxSpeed((rand() % 200) + 100);
  ////      int s = 1000;
  ////      stepper.setMaxSpeed(s);
  ////      ////    stepper.setAcceleration((rand() % 200) + 100);
  ////      stepper.setAcceleration(9999);
  ////    }
  ////  }

  //  // run
  //
  stepper.run();
//  stepper.runSpeed();
}

void onInit() {
  if (isSwitchPressed) {
    stepper.setCurrentPosition(0);
    stepper.move(0);

    delay(3000);

    state = READY;
    stepper.moveTo(-200);
    
    return;
  }

  if (stepper.distanceToGo() != 0) return;

  int s = 100;
//  stepper.setMaxSpeed(1000);
//  stepper.setSpeed(s);
//  stepper.setAcceleration(1000);
  stepper.move(100);
}
