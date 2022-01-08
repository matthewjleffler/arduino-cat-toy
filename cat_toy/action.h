#ifndef action_h
#define action_h

#include "servomanager.h"

enum ActionType
{
  pause,  // Wait, do nothing
  bob,    // Sit in place, bobbing up and down a number of times
  swing,  // Normal swing + bob
  swish,  // Swish rapidly back and forth several times
  wiggle, // Small movements in place

  stop, // Stopped - keep this last to pick new enum
};

class Action
{
public:
  void init(ServoManager *bottom, ServoManager *top);
  void update(float dt);
  void start();
  void stop();

private:
  void pickNewType();
  void setType(ActionType type);
  void fillRandom(ActionType type, int count);
  ActionType randomType();
  float pickNewRangeForTopServo(float minRange, float maxRange);

  void bobServos();
  void swingServos();
  void swishServos();
  void wiggleServos();

  ServoManager *servoBottom;
  ServoManager *servoTop;
  ActionType actionType;
  int count = -1;
  float timer = -1;
  int randomActionLength;
  ActionType randomActionArray[ActionType::stop * 3]; // 3x the max number of types
};

#endif //action_h