#include <Arduino.h>

int blinkzeitms = 1000;
unsigned long Zeitlastswitch = 0;

void setup() {
  pinMode (30, OUTPUT);
}

void loop() {
  if (millis () - Zeitlastswitch >= blinkzeitms )
    {
    int ledstate = digitalRead (30);
    digitalWrite(30, !ledstate);
    Zeitlastswitch = millis ();
  }
}

