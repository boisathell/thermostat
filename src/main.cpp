#include "Adafruit_Sensor.h"
#include "DHT.h"

#define DHTPIN 7
#define DHTTYPE DHT11

#define HEAT_PUMP 8
#define REVERSE 9
#define FAN 10

#define NUM_SENS_SAMPLES 10
#define NUM_TEMP_SAMPLES 10
#define SMOOTH_FAC (2 / (1 + NUM_TEMP_SAMPLES))

#define COOL_ABOVE 73
#define HEAT_BELOW 68
#define MARGIN 2

enum Mode { OFF, COOLING, HEATING };
Mode mode;

DHT dht(DHTPIN, DHTTYPE);

float avg = 0;
float temp = 72; // vaguely reasonable starting value

float sampleTemp() {
    avg = 0;
    for (int i = 0; i < NUM_SENS_SAMPLES; i++) {
        avg += dht.readTemperature(true);
        delay(10);
    }
    avg /= NUM_SENS_SAMPLES;

    return avg * SMOOTH_FAC + temp * (1 - SMOOTH_FAC); // ema filter
}

void off() {
    digitalWrite(FAN, LOW);
    digitalWrite(HEAT_PUMP, LOW);
    digitalWrite(REVERSE, LOW);
}

void cool() {
    digitalWrite(FAN, HIGH);
    digitalWrite(HEAT_PUMP, HIGH);
    digitalWrite(REVERSE, HIGH);
}

void heat() {
    digitalWrite(FAN, HIGH);
    digitalWrite(HEAT_PUMP, HIGH);
    digitalWrite(REVERSE, LOW);
}

void print() {
    Serial.print("Temp:");
    Serial.print(temp);

    Serial.print("\tState:");
    if (mode == OFF)
        Serial.print("OFF");
    else if (mode == COOLING)
        Serial.print("COOLING");
    else if (mode == HEATING)
        Serial.print("HEATING");

    Serial.println("");
}

void setup() {
    Serial.begin(9600);
    dht.begin();

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(FAN, OUTPUT);
    pinMode(HEAT_PUMP, OUTPUT);
    pinMode(REVERSE, OUTPUT);

    off();

    for (int i = 0; i < NUM_TEMP_SAMPLES; i++)
        temp = sampleTemp();

    mode = OFF;
}

void loop() {
    temp = sampleTemp();

    if (temp > COOL_ABOVE) {
        mode = COOLING;
        cool();
    } else if (temp < HEAT_BELOW) {
        mode = HEATING;
        heat();
    }

    if ((mode == COOLING && temp < COOL_ABOVE - MARGIN) ||
        (mode == HEATING && temp > HEAT_BELOW + MARGIN)) {
        mode = OFF;
        off();
    }

    print();
}
