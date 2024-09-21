#include <DHT.h>
#include "WifiConnectionManager.h"
#include "DHTManager.h"
#include "ServoManager.h"
#include "MQTTConnectionManager.h"

// set the SSID for the network
const char *SSID = "Ace";
const char *PASSWORD = "BF4sniperM@in";
WifiConnectionManager wifi(SSID, PASSWORD);

// set the sensor pin and type
const unsigned short DHT_PIN = 13;
const unsigned short DHTYPE = DHT11;
DHTManager dht_sensor(DHT_PIN, DHTYPE);

// set the servo pin
const unsigned short SERVO_PIN = 12;
ServoManager servo(SERVO_PIN);

// set the server, port, conection cooldown,
// username, password, and themes for mqtt
// "broker.hivemq.com"
const char *mqtt_broker = "192.168.205.1";
const unsigned short port = 1883;
const unsigned long cooldown = 1000;
const char *mqtt_username = "";
const char *mqtt_password = "";
const std::vector<std::string> themes_pub{
  "greenhouse/temperature",
  "greenhouse/humidity",
  "greenhouse/soil",
  "greenhouse/rain",
  "greenhouse/servo/out",
};
const std::vector<std::string> themes_sub{ "greenhouse/servo/in" };
MQTTConnectionManager mqtt(
  mqtt_broker,
  port,
  cooldown,
  mqtt_username,
  mqtt_password);

// set the pins for the soil and rain sensors
// those are just for show
#define SOIL_SENSOR 39
#define RAIN_SENSOR 34

// set the led pin
#define LED_PIN 4
// ** DO NOT EDIT ** variable to store the temperature
float temperature = 0;
// set the temperature that's going to be used to trip the led
const float trigger_temperature = 32;

// parameters for using non-blocking delay
unsigned long previousMillis = 0;   // ** DO NOT EDIT **
const long interval = 1000;

// user provided callback to handle mqtt requests
void mqttCallback(char *mqtt_topic, byte *payload, unsigned int length) {
  // create a callback to read input data
  Serial.print("Message arrived on topic: ");
  Serial.println(mqtt_topic);

  // construct the data using the char[] that comes from the package payload
  String data;
  for (int i = 0; i < length; i++) {
    data += (char)payload[i];
  }

  /* INFO:
    if you want to add actions for more sensors you can do it below this comment
  */

  // response to the servo input data
  // using strcmp to compare instead of just ==
  // cause it adds /0 at the end automatically
  if (!strcmp(mqtt_topic, "greenhouse/servo/in")) {
    int angle = data.toInt();
    Serial.print("Moving servo to: ");
    Serial.println(angle);
    servo.move(angle);
  }
  Serial.println("-----------------------------------");
}

void setup() {
  Serial.begin(115200);      // Initialize the serial connection
  wifi.begin();              // Initialize the connection manager
  dht_sensor.begin();        // Initialize the DHT sensor manager
  servo.begin();             // Initialize the servo manager
  mqtt.begin(mqttCallback);  // Initialize the MQTT manager
  pinMode(SOIL_SENSOR, INPUT);
  pinMode(RAIN_SENSOR, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  analogReadResolution(10);  // change the read resolution to 10bits if needed
}

void loop() {
  // connect and subscribe to themes
  mqtt.control(themes_sub);
  // trigger the led if temperature is higher than a certain threshold
  if (temperature >= trigger_temperature) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  unsigned long currentMillis = millis();            // Read current time
  if (currentMillis - previousMillis >= interval) {  // If current time - last time > 1000ms
    previousMillis = currentMillis;

    // read servo data
    unsigned short angle = servo.getAngle();
    Serial.print("Servo angle: ");
    Serial.println(angle);

    // read the temperature and humidity
    // if either returns -100 then there was an error
    temperature = dht_sensor.temperature();
    float humidity = dht_sensor.humidity();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // read the soil humidity and rain
    float soil_h = map(analogRead(SOIL_SENSOR), 0, 1023, 100, 0);
    float rain = map((analogRead(RAIN_SENSOR)), 0, 1023, 100, 0);
    Serial.print("Soil humidity: ");
    Serial.print(soil_h);
    Serial.println(" %");
    Serial.print("Rain: ");
    Serial.print(rain);
    Serial.println(" %");

    // publish the data
    mqtt.pub("greenhouse/temperature", temperature);
    mqtt.pub("greenhouse/humidity", humidity);
    mqtt.pub("greenhouse/soil", soil_h);
    mqtt.pub("greenhouse/rain", rain);
    mqtt.pub("greenhouse/servo/out", angle);
    Serial.println("-----------------------------------");
  }
}
