#ifndef tween_h
#define tween_h

class Tween
{
public:
  void tween(float from, float to, float duration);
  float update(float dt);
  bool isRunning();

private:
  float lerp(float t, float b, float c, float d);

  float from;
  float change;
  float duration = 1;
  float timer = 1;
};

#endif //tween_h