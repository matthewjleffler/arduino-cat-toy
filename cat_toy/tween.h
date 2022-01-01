#ifndef tween_h
#define tween_h

enum TweenType
{
  linear,
  out,
  in,
};

class Tween
{
public:
  void tween(float from, float to, float duration, TweenType type);
  float update(float dt);
  bool isRunning();

private:
  float lerp(float t, float b, float c, float d);
  float easeIn(float t, float b, float c, float d);
  float easeOut(float t, float b, float c, float d);

  TweenType type;
  float from;
  float change;
  float duration = 1;
  float timer = 1;
};

#endif //tween_h