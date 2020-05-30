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

//

int count = 0;
byte sensorValue = 0;

bool isHoge = false;

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

  stepper.setMaxSpeed(1000.0);
  stepper.setAcceleration(1000.0);
  stepper.runToNewPosition(-200);
}

void requestEvent(int a)
{
  sensorValue = 1;
  // Serial.println("req");
  // Serial.println("");
  // Serial.println("");
  // Serial.println("");
  // Serial.println("a");

  if (Wire.available())
  {
  // Serial.println("b");
    byte newValue = Wire.read();
    if(newValue<250){
      sensorValue = newValue;
    }
    Serial.println(sensorValue);
    // sensorValue = 2;
    // Wire.write(1);
  }
  // sensorValue = (int)c;
  // sensorValue = c;
  // sensorValue = 1;
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
    // Serial.println("init");
    doInit();
    run();
    break;
  case STATE_RESET:
    // Serial.println("reset");
    doReset();
    run();
    break;
  case STATE_READY:
    // Serial.println("ready");
    doReady();
    run();
    break;
  case STATE_WAIT:
    // Serial.println("wait");
    doWait();
    run();
    break;
  }
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
    stepper.setMaxSpeed(3000);
    stepper.setAcceleration(3000);
    stepper.move(400); // スイッチに当たったら止まるので、とりあえずガっと適当に回す、相対値
  }
}

/*----------------------------------------
 * ready
 * 待機座標に移動
 ----------------------------------------*/

void doReset()
{
  delay(3000);

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
  if (stepper.distanceToGo() == 0)
  {
    if (random(0.0, 100.0) < 10.0)
    {
      // Serial.println("slow");
      stepper.moveTo(stepper.currentPosition() + random(0.0, 3.0)); // 絶対値
      stepper.setMaxSpeed(1);
    }
    else
    {
      // Serial.println("speed");
      int range = 60;
      direction *= -1;
      //stepper.moveTo(random(WAIT_POSITION - range, WAIT_POSITION + range)); // 絶対値
      stepper.moveTo(WAIT_POSITION + random(0, range) * direction); // 絶対値
      // stepper.moveTo(WAIT_POSITION + range * direction); // 絶対値
      // stepper.setMaxSpeed(random(1000, 5000));
      stepper.setMaxSpeed(10000);
      // stepper.setAcceleration(random(1000, 5000));
      stepper.setAcceleration(10000);
      // delay(2000);
    }
  }
}

// int direction = 1;