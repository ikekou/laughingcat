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
const int STATE_RESET = 2;
const int STATE_READY = 3;
const int STATE_WAIT = 4;

/*----------------------------------------
 * setup
 ----------------------------------------*/

void setup()
{

  // state

  state = STATE_INIT;

  // serial

  Serial.begin(9600);
  Serial.println('a');

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
  // sensor.startContinuous();

  // high speed mode
  sensor.setMeasurementTimingBudget(20000);

  Serial.println('b');

  // motor


  Serial.println('c');

  // Serial.println(stepper.maxSpeed());
  // Serial.println(stepper.speed());

  Serial.println('d');

  stepper.setMaxSpeed(1000.0);
  stepper.setAcceleration(1000.0);
  stepper.runToNewPosition(-100);

  Serial.println('e');
}

/*----------------------------------------
 * loop
 ----------------------------------------*/

void loop()
{
  // Serial.println(state);

  // sensor

  // int sensorValue = sensor.readRangeContinuousMillimeters();
  //int sensorValue = sensor.readRangeSingleMillimeters();
  // bool isNear = sensorValue < NEAR;

  // switch

  int switchValue = digitalRead(SWITCH_PIN);
  isSwitchPressed = switchValue == 0;

  // state

  switch (state)
  {
  case STATE_INIT:
    doInit();
    run();
    break;
  case STATE_RESET:
    doReset();
    run();
    break;
  case STATE_READY:
    doReady();
    run();
    break;
  case STATE_WAIT:
    doWait();
    run();
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
  // stepper.runSpeedToPosition();
  // stepper.runSpeed();
}

/*----------------------------------------
 * init
 * 初期化、リセットスイッチでゼロ位置リセット
 ----------------------------------------*/

void doInit()
{
  if (isSwitchPressed)
  {
    Serial.println("init end");
    state = STATE_RESET;
    return;
  }

  if (stepper.distanceToGo() == 0){
    Serial.println("init start");
    // stepper.setMaxSpeed(1000.0);
    // stepper.setAcceleration(1000.0);

    stepper.setMaxSpeed(1000);
    stepper.setAcceleration(1000);
    stepper.move(200); // スイッチに当たったら止まるので、とりあえずガっと適当に回す、相対値

    Serial.println(stepper.speed());
    return;
  }
}

/*----------------------------------------
 * ready
 * 待機座標に移動
 ----------------------------------------*/

void doReset()
{
  delay(1000);

  stepper.setCurrentPosition(0);

  stepper.moveTo(WAIT_POSITION);

  state = STATE_READY;
}

/*----------------------------------------
 * ready
 * 待機座標に移動中
 ----------------------------------------*/

void doReady()
{
  if (stepper.distanceToGo() != 0)
    return;

  delay(1000);

  state = STATE_WAIT;
}

/*----------------------------------------
 * wait
 * 待機座標近辺でゆらゆらしながら待ち受ける
 ----------------------------------------*/

void doWait()
{
  if (stepper.distanceToGo() == 0){
    Serial.println("change");
    // stepper.moveTo(WAIT_POSITION + rand() % 30);
    int range = 45;
    stepper.moveTo(random(WAIT_POSITION-range,WAIT_POSITION+range)); // 絶対値
    stepper.setMaxSpeed(random(100,3000));
    stepper.setAcceleration(random(100,3000));
	  // stepper.setMaxSpeed((rand() % 30) + 1);
	  // stepper.setAcceleration((rand() % 30) + 1);
    Serial.println(stepper.speed());
  }
}
