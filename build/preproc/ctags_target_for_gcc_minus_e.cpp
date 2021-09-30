# 1 "/home/devil/Documents/thermostat/thermostat.ino"
# 2 "/home/devil/Documents/thermostat/thermostat.ino" 2







static const float target = 70;
static const float margin = 1;

DHT dht(7 /* Digital pin connected to the DHT sensor*/, 11 /**< DHT TYPE 11 */);

void setup() {
  Serial.begin(9600);

  pinMode(10, 0x1);
  pinMode(8, 0x1);
  pinMode(9, 0x1);

  digitalWrite(8, 0x0);
  digitalWrite(9, 0x0);
  digitalWrite(10, 0x0);

  dht.begin();
}

void loop() {
  delay(1000);

  float f = dht.readTemperature(true);
  Serial.println(f);

  // too hot
  if (target + margin < f) {
    digitalWrite(8, 0x1);
    digitalWrite(10, 0x1);
    digitalWrite(9, 0x1);
  }

  // too cold
  if (target - margin > f) {
    digitalWrite(8, 0x0);
    digitalWrite(10, 0x0);
    digitalWrite(9, 0x0);
  }

}
