#ifndef servomanager_h
#define servomanager_h

#include <Servo.h>
#include "tween.h"

class ServoManager
{
public:
  void init(int pin, int min, int max);
  void update(float dt);
  void setAngle(float normalized, float duration);
  void queueAngle(float normalized, float duration);
  bool isRunning();

private:
  Servo servo;
  Tween tween;
  float angle;
  float angleQueue;
  float angleDuration;
  int min;
  int max;
  int mid;
  int range;
};

#endif //servomanager_h