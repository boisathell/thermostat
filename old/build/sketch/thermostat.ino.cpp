#include <Arduino.h>
#line 1 "/home/devil/Documents/thermostat/thermostat.ino"
#include "DHT.h"
#define DHTPIN 7     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11

#define HEAT_PUMP 8
#define REVERSE 9
#define FAN 10

static const float target = 70;
static const float margin = 1;

DHT dht(DHTPIN, DHTTYPE);

#line 14 "/home/devil/Documents/thermostat/thermostat.ino"
void setup();
#line 28 "/home/devil/Documents/thermostat/thermostat.ino"
void loop();
#line 14 "/home/devil/Documents/thermostat/thermostat.ino"
void setup() {
  Serial.begin(9600);

  pinMode(FAN, OUTPUT);
  pinMode(HEAT_PUMP, OUTPUT);
  pinMode(REVERSE, OUTPUT);

  digitalWrite(HEAT_PUMP, LOW);
  digitalWrite(REVERSE, LOW);
  digitalWrite(FAN, LOW);
  
  dht.begin();
}

void loop() {
  delay(1000);

  float f = dht.readTemperature(true);
  Serial.println(f);

  // too hot
  if (target + margin < f) {
    digitalWrite(HEAT_PUMP, HIGH);
    digitalWrite(FAN, HIGH);
    digitalWrite(REVERSE, HIGH);
  }

  // too cold
  if (target - margin > f) {
    digitalWrite(HEAT_PUMP, LOW);
    digitalWrite(FAN, LOW);
    digitalWrite(REVERSE, LOW);
  }
  
}

