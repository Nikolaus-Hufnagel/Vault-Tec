//Librarys
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



//RFID
MFRC522 rfid(53,5); // RFID Empfänger benennen und Pins zuordnen
byte readcard[4]; //array mit ausgelesener RFID



LiquidCrystal lcd(8,7,48,46,49,47);   //LCD Pins



unsigned long blinkzeitms = 100; //Blinkzeit der roten LED bei tastendruck
unsigned long Zeitlastswitch = 0;
bool status = false;            // Status bool
int zeile=1;                    //Wechselt zwischen 0 und 1 für Emoji-Zeilensprung bei Melody
byte codes[][4] ={              //bekannte RFID Bibliothek
  {182, 73, 139, 142},
  {182, 73, 139, 141},
  {182, 73, 139, 143}
};
const byte numcodes = sizeof(codes) / sizeof(codes[0]); // Errechnet wie viele Codes in der Bibliothek sind



//Setup
void setup() {
  pinMode (30, OUTPUT); //Rote LED
  pinMode (26, OUTPUT); //Grüne LED für RFID
  pinMode (34, OUTPUT); //Rote LED für RFID
  lcd.begin(16,2);      //16 Zeichen 2 Reihen
  Serial.begin(9600);
  while (!Serial);

  SPI.begin();
  rfid.PCD_Init();
  delay(10);
}

//Methoden
void wait(unsigned long dauer){
  unsigned long pausestart = millis ();
  while (millis () - pausestart <= dauer)
  {
    noTone(42);
  }
}

void tonfalsch(){
  tone(42,220,90);
  delay(120);
  tone(42,220,90);
  delay(1000);
}


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

//Methode zum Abgleich von zwei Arrays
bool compareArrays(byte array1[], byte array2[], byte length){
  for (byte i=0; i<length; i++){
    if(array1[i] != array2[i]){
      return false;
    }
  }
  return true;
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
      readcard[i] = rfid.uid.uidByte[i];  //Array mit RFID (4 Zahlenblöcke)
      Serial.print(readcard[i],DEC);
      Serial.print(" ");
    }
  Serial.println();

  if (status == false)
  {
    for (byte i=0; i<numcodes; i++){
      if (compareArrays(readcard, codes[i],4) == true){
        status = true;
        tone(42,784,500);
        return;
      }
    }
    tonfalsch();
  }
}

//Vergleich die Codes der Karte mit den bekannten Code
/*void getIDVergleich(){


  for (byte i=0; i<numcodes; i++){
    if (compareArrays(readcard, codes[i],4) == true){
      status = true;
      tone(42,784,500);
      break;
      }
    }
    tone(42, 440, 100);
    delay(200);
    tone(42, 440, 100);
  
}*/







// Läuft dauerhaft durch im späteren Code
void loop() {
  lcd.setCursor(0,0);     //Setzt startpunkt auf Display fest
  lcd.print("VAULT-TEC");

  getID();
  //getIDVergleich();
  
  Taste = Tastenfeld.getKey(); //Mit Unter der Variablen pressedKey entspricht der gedrückten Taste

  if (Taste && Taste != 'A') {    //Bei Tastendruck wird zeit gemerkt
    Zeitlastswitch = millis (); 
    Serial.print("Lets go ");
  } 

  if ( millis () - Zeitlastswitch <= blinkzeitms) {   //Rote LED leuchtet für "blinkzeitms" millisekunden nach tastendruck
    digitalWrite(30, HIGH);
    tone(42, 600, blinkzeitms - 30);
  } 
  else { 
    digitalWrite(30, LOW);
    //noTone(42);
  }

  if (Taste == 'A') {       //Wenn Taste A gedrückt wird startet die Melody
    melody ();
  } 

  if (Taste == 'B') {       //Wenn Taste B gedrückt wird status auf false gesetzt zu test zwecken
    status = false;
  }


  if(status == false) {      //LED wechselt zwischen grün und rot abhängig vom Status
    digitalWrite(34, HIGH);
    digitalWrite(26, LOW);
  } else{
    digitalWrite(34, LOW);
    digitalWrite(26, HIGH);
  }

} 