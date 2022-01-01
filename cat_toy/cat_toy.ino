#include "servomanager.h"
#include "random.h"
#include "action.h"

enum StateRunning
{
  stopped,
  running,
};

const int buttonPin = 2;
const int servoBottomPin = 9;
const int servoTopPin = 10;

// 0-180 for servo (exclusive)
const int bottomServoMinAngle = 90;
const int bottomServoMaxAngle = 120;
const int topServoMinAngle = 55;
const int topServoMaxAngle = 180 - topServoMinAngle;

StateRunning stateRunning;
ServoManager servoBottom;
ServoManager servoTop;
Action action;
bool buttonPressed = false;
int lastTime;

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  randomSeed(analogRead(0));
  servoBottom.init(servoBottomPin, bottomServoMinAngle, bottomServoMaxAngle);
  servoTop.init(servoTopPin, topServoMinAngle, topServoMaxAngle);
  action.init(&servoBottom, &servoTop);
  setStateRunning(StateRunning::stopped);
}

void loop()
{
  int currentTime = millis();
  int change = currentTime - lastTime;
  lastTime = currentTime;

  if (change > 0)
  {
    float dt = (float)change / 1000.0;
    update(dt);
  }
}

void update(float dt)
{
  // Test input
  bool buttonFresh = testButtonPressed();
  if (buttonFresh)
  {
    switch (stateRunning)
    {
    case StateRunning::running:
      setStateRunning(StateRunning::stopped);
      break;
    case StateRunning::stopped:
      setStateRunning(StateRunning::running);
      break;
    }
  }

  switch (stateRunning)
  {
  case StateRunning::running:
    action.update(dt);
    break;
  }

  servoBottom.update(dt);
  servoTop.update(dt);
}

void setStateRunning(StateRunning newState)
{
  Serial.print("setStateRunnning: ");
  Serial.println(newState);

  stateRunning = newState;
  switch (stateRunning)
  {
  case StateRunning::stopped:
    action.stop();
    break;
  case StateRunning::running:
    action.start();
    break;
  }
}

bool testButtonPressed()
{
  bool lastButton = buttonPressed;
  buttonPressed = digitalRead(buttonPin);
  if (buttonPressed && !lastButton)
  {
    return true;
  }
  return false;
}
