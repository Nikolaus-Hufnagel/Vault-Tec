#include <Arduino.h>
#include <Keypad.h>           //Keypad Library
#include <MFRC522.h>          //RFID Library
#include <LiquidCrystal.h>    //LCD Library    
#include <Wire.h>


    //Hier wird die größe des Keypads definiert
const byte COLS = 4; //4 Spalten
const byte ROWS = 4; //4 Zeilen
    //Die Ziffern und Zeichen des Keypads werden eingegeben:
char hexaKeys[ROWS][COLS]={
{'D','#','0','*'},
{'C','9','8','7'},
{'B','6','5','4'},
{'A','3','2','1'} };
byte colPins[COLS] = {35,33,31,29}; //Definition der Pins für die 4 Spalten
byte rowPins[ROWS] = {27,25,23,22};//Definition der Pins für die 4 Zeilen
char Taste; //Taste ist die Variable für die jeweils gedrückte Taste.
Keypad Tastenfeld = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); //Das Keypad kann absofort mit "Tastenfeld" angesprochen werden



unsigned long blinkzeitms = 100; //Blinkzeit der roten LED bei tastendruck
unsigned long Zeitlastswitch = 0;




MFRC522 rfid(53,5); // RFID Empfänger benennen und Pins zuordnen
byte readcard[4];



LiquidCrystal lcd(8,7,48,46,49,47);   //LCD Pins




void setup() {
  pinMode (30, OUTPUT); //Rote LED
  lcd.begin(16,2);      //16 Zeichen 2 Reihen
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

int zeile=1;

void holdtone(int a,int note,unsigned long dauer){
  lcd.clear();
  unsigned long melodystart = millis ();
  lcd.setCursor(random(0,15),zeile);     //Setzt startpunkt auf Display fest
  lcd.print("R)");
  while (millis () - melodystart <= dauer)
  {
    tone(a,note);
  }
  zeile = !zeile;
}

void melody(){    //Fluch der Karibik

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
  lcd.clear();
}

int getID() {   //Methode um RFID auszulesen
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }
   if (!rfid.PICC_ReadCardSerial()) {
    return;
  }
  
  Serial.print("UID: ");
  
  for (byte i = 0; i < rfid.uid.size; i++) {
      readcard[i] = rfid.uid.uidByte[i];
      Serial.print(readcard[i],DEC);
      Serial.print(" ");
    }
  Serial.println();
}





void loop() {
  lcd.setCursor(0,0);     //Setzt startpunkt auf Display fest
  lcd.print("VAULT-TEC");

  getID();
  
  Taste = Tastenfeld.getKey(); //Mit Unter der Variablen pressedKey entspricht der gedrückten Taste
  
  if (Taste && Taste != 'A') {
    Zeitlastswitch = millis (); 
    Serial.print("Lets go ");
  } 

  if ( millis () - Zeitlastswitch <= blinkzeitms)
  {
    digitalWrite(30, HIGH);
    tone(42, 600);
  } 
  else { 
    digitalWrite(30, LOW);
    noTone(42);
  }

  if (Taste == 'A') {
    melody ();
  } 
  

} 