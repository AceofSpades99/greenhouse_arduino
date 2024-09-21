#ifndef DHTManager_h
#define DHTManager_h

#include "Arduino.h"

class DHTManager {
  private:

  public:
    DHTManager(const unsigned short DHTPIN, const unsigned short DHTTYPE);
    void begin();
    float temperature();
    float humidity();
};
#endif