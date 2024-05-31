#include <Arduino.h>
#include <Keypad.h>
#include <MFRC522.h>

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
byte rowPins[ROWS] = {27,25,23,22};//Definition der Pins für die 4 Zeilen
char Taste; //Taste ist die Variable für die jeweils gedrückte Taste.

Keypad Tastenfeld = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); //Das Keypad kann absofort mit "Tastenfeld" angesprochen werden

unsigned long blinkzeitms = 100;
unsigned long Zeitlastswitch = 0;



MFRC522 rfid(53,5); // RFID Empfänger benennen und Pins zuordnen


void setup() {
  pinMode (30, OUTPUT); //Rote LED
  
  Serial.begin(9600);
  while (!Serial);

  SPI.begin();
  rfid.PCD_Init();
  delay(10);
}


void wait(unsigned long dauer){
  unsigned long pausestart = millis ();
  while (millis () - pausestart <= dauer)
  {
    noTone(42);
  }
}

void holdtone(int a,int note,unsigned long dauer){
  unsigned long melodystart = millis ();
  while (millis () - melodystart <= dauer)
  {
    tone(a,note);
  }
}

void melody(){
  Serial.println("☠");
  int viertel = 240;
  int achtel = 80;
  int pause = 50;
  holdtone(42,440,achtel);
  wait(pause);
  holdtone(42,523,achtel);
  wait(pause);
  holdtone(42,587,viertel);
  wait(pause);
  holdtone(42,587,viertel);
  wait(pause);
  holdtone(42,587,achtel);
  wait(pause);
  holdtone(42,659,achtel);
  wait(pause);
  holdtone(42,698,viertel);
  wait(pause);
  holdtone(42,698,viertel);
  wait(pause);
  holdtone(42,698,achtel);
  wait(pause);
  holdtone(42,784,achtel);
  wait(pause);
  holdtone(42,659,viertel);
  wait(pause);
  holdtone(42,659,viertel);
  wait(pause);
  holdtone(42,587,achtel);
  wait(pause);
  holdtone(42,523,achtel);
  wait(pause);
  holdtone(42,523,achtel);
  wait(pause);
  holdtone(42,587,achtel);
  wait(pause);
}







void loop() {
  /*if (millis () - Zeitlastswitch >= blinkzeitms )
    {
    int ledstate = digitalRead (30);
    digitalWrite(30, !ledstate);
    Zeitlastswitch = millis (); 
  } */

  String WertDEZ;
  
  Taste = Tastenfeld.getKey(); //Mit Unter der Variablen pressedKey entspricht der gedrückten Taste
  
  if (Taste && Taste != 'A') {
    Zeitlastswitch = millis (); 
    Serial.print("Lets go ");
  } 

  if (rfid.PICC_IsNewCardPresent())
  {
    // Dezimal-Wert in Strings schreiben
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    // String zusammenbauen
    WertDEZ = WertDEZ + String(rfid.uid.uidByte[i], DEC) + " ";
  }

  // Kennung dezimal anzeigen
  Serial.println("Dezimalwert: " + WertDEZ);
  }

  if ( millis () - Zeitlastswitch <= blinkzeitms)
  {
    digitalWrite(30, HIGH);
    //digitalWrite(42, HIGH);
    tone(42, 600);
  } 
  else { 
    digitalWrite(30, LOW);
    //digitalWrite(42,LOW);
    noTone(42);
  }

  if (Taste == 'A') {
    melody ();
  } 
  

} 





