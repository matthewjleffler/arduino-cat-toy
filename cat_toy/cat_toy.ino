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

// Shutdown timer
const float shutdownTimerMaxS = 30 * 60; // 30 minutes
const float buttonPressShutdownTimerS = 5;

StateRunning stateRunning;
ServoManager servoBottom;
ServoManager servoTop;
Action action;
bool buttonPressed;
bool buttonPressStartedWhileRunning;
float buttonPressTimer;
int lastTime;
float shutdownTimer;

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
  testButtonInput(dt);

  switch (stateRunning)
  {
  case StateRunning::running:
    shutdownTimer -= dt;
    if (shutdownTimer < 0)
    {
      setStateRunning(StateRunning::stopped);
    }
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
    shutdownTimer = shutdownTimerMaxS;
    action.start();
    break;
  }
}

void testButtonInput(float dt)
{
  bool lastButton = buttonPressed;
  buttonPressed = digitalRead(buttonPin);
  bool buttonFresh = buttonPressed && !lastButton;
  bool buttonReleased = !buttonPressed && lastButton;

  if (buttonFresh)
  {
    Serial.println("Button Pressed");
  }

  if (buttonReleased)
  {
    Serial.println("Button Released");
    // Clear this flag
    buttonPressStartedWhileRunning = false;
    buttonPressTimer = 0;
  }

  if (buttonPressed)
  {
    buttonPressTimer += dt;
  }

  switch (stateRunning)
  {
  case StateRunning::stopped:
    if (buttonFresh)
    {
      setStateRunning(StateRunning::running);
    }
    break;
  case StateRunning::running:
    if (buttonFresh)
    {
      buttonPressStartedWhileRunning = true;
    }
    if (buttonPressed &&
        buttonPressStartedWhileRunning &&
        buttonPressTimer >= buttonPressShutdownTimerS)
    {
      Serial.println("Shutdown button held for long period.");
      setStateRunning(StateRunning::stopped);
    }
    break;
  }
}
