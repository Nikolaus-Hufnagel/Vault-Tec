#include <Arduino.h>
#include <Keypad.h>

    //Hier wird die größe des Keypads definiert
const byte COLS = 4; //4 Spalten
const byte ROWS = 4; //4 Zeilen
    //Die Ziffern und Zeichen des Keypads werden eingegeben:
char hexaKeys[ROWS][COLS]={
{'D','#','0','*'},
{'C','9','8','7'},
{'B','6','5','4'},
{'A','3','2','1'}
};

byte colPins[COLS] = {35,33,31,29}; //Definition der Pins für die 4 Spalten
byte rowPins[ROWS] = {27,25,23,21};//Definition der Pins für die 4 Zeilen
char Taste; //Taste ist die Variable für die jeweils gedrückte Taste.

Keypad Tastenfeld = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); //Das Keypad kann absofort mit "Tastenfeld" angesprochen werden

int blinkzeitms = 1000;
unsigned long Zeitlastswitch = 0;
boolean leuchte = false;

void setup() {
  pinMode (30, OUTPUT);
  
  
}







void loop() {
  /*if (millis () - Zeitlastswitch >= blinkzeitms )
    {
    int ledstate = digitalRead (30);
    digitalWrite(30, !ledstate);
    Zeitlastswitch = millis (); 
  } */
  
  Taste = Tastenfeld.getKey(); //Mit Unter der Variablen pressedKey entspricht der gedrückten Taste
  
  if (Taste) {
    leuchte = true;
    Zeitlastswitch = millis (); 
    
  } 
  if ( millis () - Zeitlastswitch <= blinkzeitms)
  {
    digitalWrite(30, HIGH);
  } else { digitalWrite(30, LOW);}
  
  
}

