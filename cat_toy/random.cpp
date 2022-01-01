#include <Arduino.h>
#include "random.h"

int Random::randomInt(int min, int max)
{
  return random(min, max);
}

float Random::randomFloat()
{
  return random(0, 10001) / 10000.0;
}

float Random::randomFloatRange(float min, float max)
{
  float rand = randomFloat() * (max - min);
  return min + rand;
}