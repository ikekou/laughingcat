#include <AccelStepper.h>
#include <Wire.h>
#include <VL53L0X.h>

// motor

AccelStepper stepper(1, 6, 5); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
int direction = 1;
int WAIT_POSITION = -200;

// switch

const int SWITCH_PIN = 18;
bool isSwitchPressed = false;

// distance sensor

VL53L0X sensor;
const int NEAR = 50;

// state

int state = 0;
const int STATE_INIT = 1;
const int STATE_READY = 2;
const int STATE_WAIT = 3;

/*----------------------------------------
 * setup
 ----------------------------------------*/

void setup()
{

  // state

  state = STATE_INIT;

  // serial

  Serial.begin(9600);

  // switch

  Wire.begin();
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  // distance sensor

  Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1)
    {
      // noop
    }
  }
  sensor.startContinuous();

  // motor

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(1000);

  Serial.println(stepper.maxSpeed());
  Serial.println(stepper.speed());
}

/*----------------------------------------
 * loop
 ----------------------------------------*/

void loop()
{

  // sensor

  int sensorValue = sensor.readRangeContinuousMillimeters();
  bool isNear = sensorValue < NEAR;

  // switch

  int switchValue = digitalRead(SWITCH_PIN);
  isSwitchPressed = switchValue == 0;

  // state

  Serial.println(state);

  swit(state)
  {
  case STATE_INIT:
    init();
    run();
    break;
  case STATE_READY:
    ready();
    break;
  case STATE_WAIT:
    wait();
    break;
  }

  // if (state == STATE_INIT)
  // {
  //   init();
  //   stepper.run();
  // }
  // else if (state == STATE_READY)
  // {
  //   //    stepper.setSpeed(100);
  //   //    stepper.setMaxSpeed(1000);
  //   ready();
  // }

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
  //  stepper.runSpeed();
}

/*----------------------------------------
 * run
 ----------------------------------------*/

void run()
{
  stepper.run();
}

/*----------------------------------------
 * init
 ----------------------------------------*/

void init()
{
  if (isSwitchPressed)
  {
    state = STATE_READY;
    return;
  }

  if (stepper.distanceToGo() != 0)
    return;

  stepper.move(100);
}

/*----------------------------------------
 * ready
 ----------------------------------------*/

void ready()
{
  stepper.setCurrentPosition(0);
  stepper.move(0);

  delay(3000);

  stepper.moveTo(WAIT_POSITION);

  state = STATE_WAIT;
}

/*----------------------------------------
 * wait
 ----------------------------------------*/

void wait()
{
  if (stepper.distanceToGo() != 0)
    return;

  // WIP...
}
