#include "tween.h"
#include <Arduino.h>

void Tween::tween(float from, float to, float duration)
{
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
