#include "servomanager.h"

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

const int minQuickBeforeLongPause = 3;
const int maxQuickBeforeLongPause = 17;

const float minSwingRateS = 0.4;
const float maxSwingRateS = 0.7;
const float minQuickDelayS = 0.45;
const float maxQuickDelayS = 0.85;
const float minLongDelayS = 1;
const float maxLongDelayS = 7;

StateRunning stateRunning;
ServoManager servoBottom;
ServoManager servoTop;
bool buttonPressed = false;
float timer;
int lastTime;
int quickCount;
int rangePhase;

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  randomSeed(analogRead(0));
  servoBottom.init(servoBottomPin, bottomServoMinAngle, bottomServoMaxAngle);
  servoTop.init(servoTopPin, topServoMinAngle, topServoMaxAngle);
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
    runServos(dt);
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
    servoBottom.setAngle(0, 0, TweenType::linear);
    servoTop.setAngle(.5, 0, TweenType::linear);
    break;
  case StateRunning::running:
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

void runServos(float dt)
{
  timer -= dt;
  if (timer > 0)
  {
    return;
  }

  float servoSwingTime = swingServos();

  if (--quickCount > 0)
  {
    // Another quick
    timer = servoSwingTime + randomFloatRange(minQuickDelayS, maxQuickDelayS);
  }
  else
  {
    // Long delay
    timer = servoSwingTime + randomFloatRange(minLongDelayS, maxLongDelayS);
    // Pick new quick count
    quickCount = random(minQuickBeforeLongPause, maxQuickBeforeLongPause);
  }
}

/**
 * @brief sets servos to swing
 *
 * @return float, how long the swing is for
 */
float swingServos()
{
  float swingTime = randomFloatRange(minSwingRateS, maxSwingRateS);
  float halfTime = swingTime / 2.0;
  float newAngle = randomFloat();

  servoBottom.setAngle(1, halfTime, TweenType::out);
  servoBottom.queueAngle(0, halfTime, TweenType::in);
  servoTop.setAngle(newAngle, swingTime, TweenType::linear);
  return swingTime;
}

float randomFloat()
{
  return random(0, 10001) / 10000.0;
}

float randomFloatRange(float min, float max)
{
  float rand = randomFloat() * (max - min);
  return min + rand;
}
