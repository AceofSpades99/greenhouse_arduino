#include <DHT.h>
#include "DHTManager.h"

DHT dht(13, DHT11);   // DHT sensor default

DHTManager::DHTManager(const unsigned short DHTPIN, const unsigned short DHTTYPE) {
  DHT dht(DHTPIN, DHTTYPE);   // DHT sensor change default status
}

void DHTManager::begin() {
  dht.begin();
}

float DHTManager::temperature() {
  // Read temperature
  float t = dht.readTemperature();
  // if it returns NAN then show an error
  if (isnan(t)) {
    Serial.println(F("Error reading temperature!"));
    return -100;
  } else {
    return t;
  }
}

float DHTManager::humidity() {
  // Read humidity
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println(F("Error reading humidity!"));
    return -100;
  } else {
    return h;
  }
}