#include <Arduino.h>
#include "action.h"
#include "random.h"

// Action constants

// Debug test a specific mode. stop means no override
const ActionType overrideType = ActionType::stop;

// Pause
const float pauseDelayMinS = 1;
const float pauseDelayMaxS = 7;

// Bob
const int bobCountMin = 2;
const int bobCountMax = 5;
const float bobRateMinS = 0.3;
const float bobRateMaxS = 0.5;
const float bobDelayMinS = 0.2;
const float bobDelayMaxS = 0.4;

// Swing
const int swingCountMin = 3;
const int swingCountMax = 17;
const float swingRateMinS = 0.4;
const float swingRateMaxS = 0.7;
const float swingDelayMinS = 0.45;
const float swingDelayMaxS = 0.85;

// Swish
const int swishCountMin = 3;
const int swishCountMax = 17;
const float swishRangeMin = 0.4;
const float swishRangeMax = 0.8;
const float swishRateMinS = 0.15;
const float swishRateMaxS = 0.6;
const float swishDelayMinS = 0.1;
const float swishDelayMaxS = 0.4;

void Action::init(ServoManager *bottom, ServoManager *top)
{
  servoBottom = bottom;
  servoTop = top;
}

void Action::update(float dt)
{
  timer -= dt;
  switch (actionType)
  {
  case ActionType::pause:
    break;
  case ActionType::bob:
    bobServos();
    break;
  case ActionType::swing:
    swingServos();
    break;
  case ActionType::swish:
    swishServos();
    break;
  }

  // Finished this action
  if (timer <= 0 && count <= 0)
  {
    pickNewType();
  }
}

void Action::start()
{
  pickNewType();
}

void Action::stop()
{
  setType(ActionType::stop);
}

void Action::pickNewType()
{
  ActionType newActionType;

  // Debug override testing
  if (overrideType != ActionType::stop)
  {
    if (actionType == overrideType)
    {
      newActionType = ActionType::pause;
    }
    else
    {
      newActionType = overrideType;
    }
  }
  // Normal random picking
  else
  {
    do
    {
      newActionType = randomType();
    } while (newActionType == lastActionType);
  }

  setType(newActionType);
}

void Action::setType(ActionType type)
{
  lastActionType = actionType;
  actionType = type;

  Serial.print("Action::startType: ");
  Serial.println(actionType);

  switch (actionType)
  {
  case ActionType::pause:
    count = -1;
    timer = Random::randomFloatRange(pauseDelayMinS, pauseDelayMaxS);
    Serial.print("Pause time: ");
    Serial.println(timer);
    break;
  case ActionType::bob:
    count = Random::randomInt(bobCountMin, bobCountMax);
    timer = -1;
    Serial.print("Bob count: ");
    Serial.println(count);
    break;
  case ActionType::swing:
    count = Random::randomInt(swingCountMin, swingCountMax);
    timer = -1;
    Serial.print("Swing count: ");
    Serial.println(count);
    break;
  case ActionType::swish:
    count = Random::randomInt(swishCountMin, swishCountMax);
    timer = -1;
    Serial.print("Swish count: ");
    Serial.println(count);
    break;
  case ActionType::stop:
    Serial.println("Stop");
    servoBottom->setAngle(0, 0, TweenType::linear);
    servoTop->setAngle(.5, 0, TweenType::linear);
    break;
  }
}

ActionType Action::randomType()
{
  // TODO(mleffler): Weighted randomness? Logical tree of available actions?
  int random = Random::randomInt(ActionType::pause, ActionType::stop);
  return (ActionType)random;
}

float Action::pickNewRangeForTopServo()
{
  // Pick new range at a distance from current angle
  float currentAngleNormalized = servoTop->getAngleNormalized();
  Serial.print("Current: ");
  Serial.print(currentAngleNormalized);
  float change = Random::randomFloatRange(swishRangeMin, swishRangeMax);
  Serial.print(" change: ");
  Serial.print(change);

  // Send it in the other direction from the half of the range we're in
  float newAngle;
  if (currentAngleNormalized >= 0.5)
  {
    newAngle = currentAngleNormalized - change;
  }
  else
  {
    newAngle = currentAngleNormalized + change;
  }
  Serial.print(" new: ");
  Serial.print(newAngle);

  // Clamp the angle
  if (newAngle < 0)
  {
    newAngle = 0;
  }
  else if (newAngle > 1)
  {
    newAngle = 1;
  }
  Serial.print(" clamped: ");
  Serial.println(newAngle);

  return newAngle;
}

void Action::bobServos()
{
  if (timer > 0 || count <= 0)
  {
    return;
  }

  count--;

  float bobTime = Random::randomFloatRange(bobRateMinS, bobRateMaxS);
  float halfTime = bobTime / 2.0;

  servoBottom->setAngle(1, halfTime, TweenType::out);
  servoBottom->queueAngle(0, halfTime, TweenType::in);

  timer = bobTime + Random::randomFloatRange(bobDelayMinS, bobDelayMaxS);

  Serial.print("Bobs remaining: ");
  Serial.print(count);
  Serial.print(" time: ");
  Serial.println(timer);
}

void Action::swingServos()
{
  if (timer > 0 || count <= 0)
  {
    return;
  }

  count--;

  float swingTime = Random::randomFloatRange(swingRateMinS, swingRateMaxS);
  float halfTime = swingTime / 2.0;
  float newAngle = pickNewRangeForTopServo();

  servoBottom->setAngle(1, halfTime, TweenType::out);
  servoBottom->queueAngle(0, halfTime, TweenType::in);
  servoTop->setAngle(newAngle, swingTime, TweenType::linear);

  timer = swingTime + Random::randomFloatRange(swingDelayMinS, swingDelayMaxS);

  Serial.print("Swing remaining: ");
  Serial.print(count);
  Serial.print(" time: ");
  Serial.println(timer);
}

void Action::swishServos()
{
  if (timer > 0 || count <= 0)
  {
    return;
  }

  count--;

  float swishTime = Random::randomFloatRange(swishRateMinS, swishRateMaxS);
  float newAngle = pickNewRangeForTopServo();

  // Assign the angle
  servoTop->setAngle(newAngle, swishTime, TweenType::linear);

  timer = swishTime + Random::randomFloatRange(swishDelayMaxS, swishDelayMaxS);

  Serial.print("Swish remaining: ");
  Serial.print(count);
  Serial.print(" time: ");
  Serial.print(timer);
  Serial.print(" angle: ");
  Serial.println(newAngle);
}