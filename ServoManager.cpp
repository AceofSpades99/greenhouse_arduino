#include <ESP32Servo.h>
#include "ServoManager.h"

ServoManager::ServoManager(const unsigned short servo_pin) {
  _servo_pin = servo_pin;
}

void ServoManager::begin() {
  // Setup servo
  _servo.attach(_servo_pin);
  _servo.write(_angle);
}

void ServoManager::move(unsigned short angle) {
  // move the servo to a specified angle between 0 and 180
  _angle = angle;
  _servo.write(angle);
}

unsigned short ServoManager::getAngle() {
  return _angle;
}
