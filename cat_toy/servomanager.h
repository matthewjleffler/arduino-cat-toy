#ifndef servomanager_h
#define servomanager_h

#include <Servo.h>
#include "tween.h"

class ServoManager
{
public:
  void init(int pin, int min, int max);
  void update(float dt);
  void setAngle(float normalized, float duration, TweenType type);
  void queueAngle(float normalized, float duration, TweenType type);
  bool isRunning();

private:
  Servo servo;
  Tween tween;
  float angle;
  float queuedAngle;
  float queuedDuration;
  TweenType queuedTween;
  int min;
  int max;
  int mid;
  int range;
};

#endif //servomanager_h