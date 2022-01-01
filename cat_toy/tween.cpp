#include "tween.h"
#include <Arduino.h>

void Tween::tween(float from, float to, float duration, TweenType type)
{
  this->type = type;
  timer = 0;
  if (duration <= 0)
  {
    duration = 1;
    timer = 1;
  }
  this->duration = duration;
  this->from = from;
  change = to - from;
}

float Tween::update(float dt)
{
  timer += dt;

  float clampedTime = timer;
  if (clampedTime < 0)
  {
    clampedTime = 0;
  }
  if (clampedTime > duration)
  {
    clampedTime = duration;
  }

  switch (type)
  {
  case TweenType::linear:
    return lerp(clampedTime, from, change, duration);
  case TweenType::out:
    return easeOut(clampedTime, from, change, duration);
  case TweenType::in:
    return easeIn(clampedTime, from, change, duration);
  }

  return lerp(clampedTime, from, change, duration);
}

bool Tween::isRunning()
{
  return timer < duration;
}

float Tween::lerp(float t, float b, float c, float d)
{
  return c * t / d + b;
}

float Tween::easeOut(float t, float b, float c, float d)
{
  return -c * (t /= d) * (t - 2) + b;
}

float Tween::easeIn(float t, float b, float c, float d)
{
  return c * (t /= d) * t + b;
}