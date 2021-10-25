#include "DHT.h"
#define DHTPIN 7     
#define DHTTYPE DHT11


#define HEAT_PUMP 8
#define REVERSE 9
#define FAN 10

#define DISABLE_RELAYS false
#define NUM_SAMPLES 10
#define SAMPLE_TIME 1000


static const float target = 72;
static const float margin = 1;
float f = target;
float builtin_f = target;
unsigned long T_0 = 0;
unsigned long UPDATE_T = 0;
static unsigned long MIN_REFRESH_INTERVAL = 2 * 60 * 1000; // 2 minutes
String data = String("");

String target_status = String("");
String fan_status = String("OFF");
String heat_pump_status = String("OFF");
static int WARNING_LED_HALF_BLINK_TIME = 50; // 10 times a second
bool warning = false;
float to_fl = 0.0; // remote decoded float value (not used anymore)

float samples[NUM_SAMPLES];

DHT dht(DHTPIN, DHTTYPE);


int push_n = 0;
void pushTemp(float t) {
  samples[push_n] = t;

  float sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum+=samples[i];
  }
  f = sum / NUM_SAMPLES;
  
  push_n++;
  push_n = push_n % NUM_SAMPLES;
}


void setup() {
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(HEAT_PUMP, OUTPUT);
  pinMode(REVERSE, OUTPUT);

  digitalWrite(HEAT_PUMP, LOW);
  digitalWrite(REVERSE, LOW);
  digitalWrite(FAN, LOW);

  dht.begin();

  // populate with 10 datapoints
  for (int i = 0; i < NUM_SAMPLES; i++) {
    delay(SAMPLE_TIME);
    pushTemp(dht.readTemperature(true));
  }

  builtin_f = f;

  T_0 = millis(); 
  UPDATE_T = T_0;
}

void updateRelays() {
  // for cooling only 
  target_status = String(target-margin) + " < T* < " + String(target+margin);
  // too hot
  if (target + margin < f) {

    if (!DISABLE_RELAYS) { 
      digitalWrite(HEAT_PUMP, HIGH);
      digitalWrite(FAN, HIGH);
      digitalWrite(REVERSE, HIGH);
    }
    heat_pump_status = String("ON, REVERSED (COOLING)");
    fan_status = String("ON");
    target_status = String(target-margin)+ " < " + String(target+margin) + " < T*";
  }

  // too cold
  if (target - margin > f) {

    if (!DISABLE_RELAYS) { 
      digitalWrite(HEAT_PUMP, LOW);
      digitalWrite(FAN, LOW);
      digitalWrite(REVERSE, LOW);
    }
    heat_pump_status = String("OFF");
    fan_status = String("OFF");
    target_status = "T* < " + String(target-margin)+ " < " + String(target+margin);
  }

}

void emitSerialUpdate() {
        Serial.print(f);
        Serial.print(' ');
        Serial.print("REMOTE=");
        Serial.print(to_fl);
        Serial.print(" INTERNAL=");
        Serial.print(builtin_f);
        Serial.print(" | STATUS: ");
        Serial.print(target_status);
        Serial.print(" | HEAT_PUMP: ");
        Serial.print(heat_pump_status);
        Serial.print(" | FAN: ");
        Serial.print(fan_status);
        Serial.print(" | WARNING: ");
        Serial.print(warning ? "ON | " : "OFF | ");
        Serial.println("");
}

void loop() {
  updateRelays();
  emitSerialUpdate();
  delay(SAMPLE_TIME);
  pushTemp(dht.readTemperature(true));
  builtin_f = f;
}
