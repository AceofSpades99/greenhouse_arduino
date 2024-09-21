#ifndef ServoManager_h
#define ServoManager_h

#include "Arduino.h"
#include <ESP32Servo.h>

class ServoManager {
  private:
    Servo _servo;
    unsigned short _servo_pin;
    unsigned short _angle = 0;
  public:
    ServoManager(const unsigned short int servo_pin);
    void begin();
    void move(unsigned short angle);
    unsigned short getAngle();
};
#endif
