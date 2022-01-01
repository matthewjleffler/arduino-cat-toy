#ifndef random_h
#define random_h

class Random
{
public:
  static int randomInt(int min, int max); // Exclusive
  static float randomFloat();
  static float randomFloatRange(float min, float max); // Inclusive
};

#endif //random_h