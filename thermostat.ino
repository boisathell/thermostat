#include "DHT.h"
#define DHTPIN 7     
#define DHTTYPE DHT11

#define HEAT_PUMP 8
#define REVERSE 9
#define FAN 10

#define DISABLE_RELAYS false


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


DHT dht(DHTPIN, DHTTYPE);


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
  f = dht.readTemperature(true);
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

void loop() {
  updateRelays();

  if (millis() - UPDATE_T > MIN_REFRESH_INTERVAL) {
    builtin_f = dht.readTemperature(true);
    f = builtin_f; 
    warning = true;
    UPDATE_T = millis();
  }

  while (Serial.available() > 0) {
    int d = Serial.read();
    if (d != -1) {
      char c = d;
      data += String(c);
      if (c == '\n') {
        float to_fl = data.toFloat();
        
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

        if (to_fl != 0) {
          if (to_fl > 10.0 && to_fl < 140.0 && abs(builtin_f - to_fl) < 30.0 ) {
            f = to_fl; 
          } else {
            f = builtin_f;
            Serial.print("  ---  Suspicious Sensor Values, Using Internal Measurement. Data Vector:");
            for (char d_n = 0; d_n < data.length(); d_n++) {
              int d_a_n = data.charAt(d_n);
              Serial.print("  ");
              Serial.print(d_a_n);
            } 
          }
        } else {
          f = builtin_f;
          Serial.print("  ---  Float Conversion Failure. Using Internal Measurement. Data Vector:");
          for (char d_n = 0; d_n < data.length(); d_n++) {
            int d_a_n = data.charAt(d_n);
            Serial.print("  ");
            Serial.print(d_a_n);
          } 
          
        }
        Serial.print('\n');
        UPDATE_T = millis();
        data = String("");
        builtin_f = dht.readTemperature(true);
      }
    }
  }  

  if (warning) {
    if (millis() % (2 * WARNING_LED_HALF_BLINK_TIME) < WARNING_LED_HALF_BLINK_TIME) {
      digitalWrite(LED_BUILTIN, HIGH); 
    } else {
      digitalWrite(LED_BUILTIN, LOW); 
    }
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  delay(10);
  

}
