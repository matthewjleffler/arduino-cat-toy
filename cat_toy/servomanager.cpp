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
  setAngle(0.5, 0);
}

void ServoManager::update(float dt)
{
  angle = tween.update(dt);
  servo.write(angle);

  // Run second half of change, if we have a queue
  if (!tween.isRunning() && angleQueue >= 0)
  {
    setAngle(angleQueue, angleDuration);
  }
}

void ServoManager::setAngle(float normalized, float duration)
{
  int newAngle = min + range * normalized;
  tween.tween(angle, newAngle, duration);
  angleQueue = -1; // Clear any queue
}

void ServoManager::queueAngle(float normalized, float duration)
{
  angleQueue = normalized;
  angleDuration = duration;
}

bool ServoManager::isRunning()
{
  return tween.isRunning();
}