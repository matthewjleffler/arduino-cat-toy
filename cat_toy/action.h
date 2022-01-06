#ifndef action_h
#define action_h

#include "servomanager.h"

enum ActionType
{
  pause, // Wait, do nothing
  bob,   // Sit in place, bobbing up and down a number of times
  swing, // Normal swing + bob
  swish, // Swish rapidly back and forth several times

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
  ActionType randomType();
  float pickNewRangeForTopServo();

  void bobServos();
  void swingServos();
  void swishServos();

  ServoManager *servoBottom;
  ServoManager *servoTop;
  ActionType actionType;
  int count = -1;
  float timer = -1;
  int randomActionLength;
  ActionType randomActionArray[ActionType::stop * 4]; // 4x the max number of types
};

#endif //action_h