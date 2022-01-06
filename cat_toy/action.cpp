#include <Arduino.h>
#include "action.h"
#include "random.h"

// Action constants

// #define LOG

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
    newActionType = randomType();
  }

  setType(newActionType);
}

void Action::setType(ActionType type)
{
  Serial.print("Action::startType: ");
  actionType = type;

  switch (actionType)
  {
  case ActionType::pause:
    count = -1;
    timer = Random::randomFloatRange(pauseDelayMinS, pauseDelayMaxS);
    Serial.print("pause time: ");
    Serial.println(timer);
    break;
  case ActionType::bob:
    count = Random::randomInt(bobCountMin, bobCountMax);
    timer = -1;
    Serial.print("bob count: ");
    Serial.println(count);
    break;
  case ActionType::swing:
    count = Random::randomInt(swingCountMin, swingCountMax);
    timer = -1;
    Serial.print("swing count: ");
    Serial.println(count);
    break;
  case ActionType::swish:
    count = Random::randomInt(swishCountMin, swishCountMax);
    timer = -1;
    Serial.print("swish count: ");
    Serial.println(count);
    break;
  case ActionType::stop:
    Serial.println("stop");
    servoBottom->setAngle(0, 0, TweenType::linear);
    servoTop->setAngle(.5, 0, TweenType::linear);
    break;
  }
}

ActionType Action::randomType()
{
  // TODO(mleffler): Fill out with weights
  switch (actionType)
  {
  case ActionType::stop:
  case ActionType::pause:
    randomActionLength = 3;
    randomActionArray[0] = ActionType::bob;
    randomActionArray[1] = ActionType::swing;
    randomActionArray[2] = ActionType::swish;
    break;
  case ActionType::bob:
    randomActionLength = 5;
    randomActionArray[0] = ActionType::pause;
    randomActionArray[1] = ActionType::swing;
    randomActionArray[2] = ActionType::swing;
    randomActionArray[3] = ActionType::swish;
    randomActionArray[4] = ActionType::swish;
    break;
  case ActionType::swing:
    randomActionLength = 5;
    randomActionArray[0] = ActionType::pause;
    randomActionArray[1] = ActionType::bob;
    randomActionArray[2] = ActionType::bob;
    randomActionArray[3] = ActionType::swish;
    randomActionArray[4] = ActionType::swish;
    break;
  case ActionType::swish:
    randomActionLength = 5;
    randomActionArray[0] = ActionType::pause;
    randomActionArray[1] = ActionType::bob;
    randomActionArray[2] = ActionType::bob;
    randomActionArray[3] = ActionType::swing;
    randomActionArray[4] = ActionType::swing;
    break;
  }

  int random = Random::randomInt(0, randomActionLength);
  return randomActionArray[random];
}

float Action::pickNewRangeForTopServo()
{
  // Pick new range at a distance from current angle
  float currentAngleNormalized = servoTop->getAngleNormalized();
  float change = Random::randomFloatRange(swishRangeMin, swishRangeMax);

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

  // Clamp the angle
  if (newAngle < 0)
  {
    newAngle = 0;
  }
  else if (newAngle > 1)
  {
    newAngle = 1;
  }

#ifdef LOG
  Serial.print("Current: ");
  Serial.print(currentAngleNormalized);
  Serial.print(" change: ");
  Serial.print(change);
  Serial.print(" new: ");
  Serial.println(newAngle);
#endif

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
  float thirdTime = thirdTime / 3.0;

  servoBottom->setAngle(1, thirdTime, TweenType::out);
  servoBottom->queueAngle(thirdTime, 0, thirdTime, TweenType::in);

  timer = bobTime + Random::randomFloatRange(bobDelayMinS, bobDelayMaxS);

#ifdef LOG
  Serial.print("Bobs remaining: ");
  Serial.print(count);
  Serial.print(" time: ");
  Serial.println(timer);
#endif
}

void Action::swingServos()
{
  if (timer > 0 || count <= 0)
  {
    return;
  }

  count--;

  float swingTime = Random::randomFloatRange(swingRateMinS, swingRateMaxS);
  float thirdTime = swingTime / 3.0;
  float newAngle = pickNewRangeForTopServo();

  servoBottom->setAngle(1, thirdTime, TweenType::out);
  servoBottom->queueAngle(thirdTime, 0, thirdTime, TweenType::in);
  servoTop->setAngle(newAngle, swingTime, TweenType::linear);

  timer = swingTime + Random::randomFloatRange(swingDelayMinS, swingDelayMaxS);

#ifdef LOG
  Serial.print("Swing remaining: ");
  Serial.print(count);
  Serial.print(" time: ");
  Serial.println(timer);
#endif
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

#ifdef LOG
  Serial.print("Swish remaining: ");
  Serial.print(count);
  Serial.print(" time: ");
  Serial.print(timer);
  Serial.print(" angle: ");
  Serial.println(newAngle);
#endif
}