#include <Arduino.h>
#include "servomanager.h"

void ServoManager::init(int pin, int min, int max)
{
  servo.attach(pin);
  this->min = min;
  this->max = max;
  range = max - min;
  mid = max / 2;
  angle = mid;
  setAngle(0.5, 0, TweenType::linear);
}

void ServoManager::update(float dt)
{
  angle = tween.update(dt);
  servo.write(angle);

  // Run second half of change, if we have a queue
  if (!tween.isRunning() && queuedAngle >= 0)
  {
    queuedDelay -= dt;
    if (queuedDelay < 0)
    {
      setAngle(queuedAngle, queuedDuration, queuedTween);
    }
  }
}

void ServoManager::setAngle(float normalized, float duration, TweenType type)
{
  destinationAngleNormalized = normalized;
  float newAngle = min + range * normalized;
  tween.tween(angle, newAngle, duration, type);
  queuedAngle = -1; // Clear any queue
}

void ServoManager::queueAngle(float delay, float normalized, float duration, TweenType type)
{
  queuedDelay = delay;
  queuedAngle = normalized;
  queuedDuration = duration;
  queuedTween = type;
}

bool ServoManager::isRunning()
{
  return tween.isRunning();
}

float ServoManager::getAngleNormalized()
{
  return destinationAngleNormalized;
}