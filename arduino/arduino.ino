#include <AccelStepper.h>
#include <Wire.h>

/*----------------------------------------
 * variables
 ----------------------------------------*/

// motor

AccelStepper stepper(1, 6, 5); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
int direction = 1;
int WAIT_POSITION = -300;

// switch

const int SWITCH_PIN = 18;
bool isSwitchPressed = false;

// sensor

const int SENSOR_PIN = A1;
const int DISTANCE_THRESHOLD = 30;

// state

int state = 0;
const int STATE_INIT = 1;
const int STATE_RESET = 2;
const int STATE_READY = 3;
const int STATE_WAIT = 4;
const int STATE_HOGE = 5;
const int STATE_ATTACKED = 6;

//

int count = 0;
byte sensorValue = 0;

bool isHoge = false;

int waitCount = 0;

/*----------------------------------------
 * setup
 ----------------------------------------*/

void setup()
{
  // Wire

  Wire.begin(8);
  Wire.onReceive(requestEvent); //割り込み関数の指定

  // state

  state = STATE_INIT;

  // serial

  Serial.begin(9600);

  // switch

  pinMode(SWITCH_PIN, INPUT_PULLUP);

  // motor

  stepper.setSpeed(1000);
  stepper.setMaxSpeed(1000.0);
  stepper.setAcceleration(1000.0);
  stepper.runToNewPosition(-200);
}

void requestEvent(int a)
{
  sensorValue = 1;

  if (Wire.available())
  {
    byte newValue = Wire.read();
    if(newValue<250){
      sensorValue = newValue;
    }
    // Serial.println(sensorValue);

    if(sensorValue > 10){
      state = STATE_ATTACKED;
      // Serial.println("attacked (real)");
    }
  }
}

/*----------------------------------------
 * loop
 ----------------------------------------*/

void loop()
{
  // Wire.requestFrom(0x1E, 1);

  // while (Wire.available())
  // {
  //   byte val = Wire.read();
  //   Serial.print(val);
  // }

  // int sensorValue = analogRead(SENSOR_PIN);
  // Serial.println(sensorValue);
  // bool isNear = sensorValue > DISTANCE_THRESHOLD;

  // if (isHoge)
  // {
  // state = STATE_INIT;
  // stepper.set
  // isHoge = false;
  // }

  // while (Wire.available())
  // {                       // 要求より短いデータが来る可能性あり
  //   char c = Wire.read(); // 1バイトを受信
  //   Serial.print(c);
  // }

  // switch

  int switchValue = digitalRead(SWITCH_PIN);
  isSwitchPressed = switchValue == 0;

  // state

  switch (state)
  {
  case STATE_INIT:
    doInit();
    break;
  case STATE_RESET:
    // Serial.println("reset");
    doReset();
    break;
  case STATE_READY:
    // Serial.println("ready");
    doReady();
    break;
  case STATE_WAIT:
    // Serial.println("wait");
    doWait();
    break;
  case STATE_ATTACKED:
    doAttacked();
    break;
  }

  // update

  run();
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
 * 初期化、リセットスイッチでゼロ位置リセット
 ----------------------------------------*/

void doInit()
{
  if (isSwitchPressed)
  {
    // Serial.println("isSwitchPressed");
    state = STATE_RESET;
    return;
  }

  if (stepper.distanceToGo() == 0)
  {
    stepper.setMaxSpeed(1000);
    stepper.setAcceleration(100);
    stepper.move(1600); // スイッチに当たったら止まるので、とりあえずガっと適当に回す、相対値
    // stepper.setSpeed(500);
  }
}

/*----------------------------------------
 * ready
 * 待機座標に移動
 ----------------------------------------*/

void doReset()
{
  delay(5000);

  stepper.setCurrentPosition(0);

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(100);
  stepper.moveTo(WAIT_POSITION);
  // stepper.setSpeed(500);

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

  // delay(1000);

  state = STATE_WAIT;

}

/*----------------------------------------
 * wait
 * 待機座標近辺でゆらゆらしながら待ち受ける
 ----------------------------------------*/

void doWait()
{
  if(waitCount>0) {
    delay(1);
      // Serial.println("waiting");
    waitCount--;
    return;
  }

  if (stepper.distanceToGo() == 0)
  {
    float randomValue = random(0.0, 100.0);

    if (randomValue < 1.0){
      // Serial.println("attacked (dummy)");
      state = STATE_ATTACKED;
      stepper.setCurrentPosition(0);
    }
    
    else if (randomValue < 10.0)
    {
      // Serial.println("wait");
      // stepper.moveTo(stepper.currentPosition() + random(0.0, 3.0)); // 絶対値
      // stepper.setMaxSpeed(1);
      //waitCount = random(300,500);
      waitCount = random(100,2000);
      Serial.println(waitCount);
    }

    else
    {
      // Serial.println("random");
      int range = 50;
      direction *= -1;
      int a = 1000;
      stepper.setMaxSpeed(a);
      stepper.setAcceleration(a);
      stepper.moveTo(WAIT_POSITION + random(0, range) * direction); // 絶対値
    }
  }
}

/*----------------------------------------
 * attacked
 * 襲われた
 ----------------------------------------*/

void doAttacked()
{
  if (isSwitchPressed)
  {
    state = STATE_RESET;
    return;
  }

  if (stepper.distanceToGo() == 0)
  {
    int s = 900;
    stepper.setMaxSpeed(s);
    stepper.setAcceleration(s*2);
    stepper.move(10000); // スイッチに当たったら止まるので、とりあえずガっと適当に回す、相対値
  }
}