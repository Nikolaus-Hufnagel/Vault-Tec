//Librarys
#include <Arduino.h>
#include <Keypad.h>           //Keypad Library
#include <MFRC522.h>          //RFID Library
#include <LiquidCrystal.h>    //LCD Library    
#include <Wire.h>
#include <Servo.h>            //Servo Library


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
Keypad Tastenfeld = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); //Das Keypad kann mit "Tastenfeld" angesprochen werden


int codecorrect[4] = {1,2,3,4}; //Der korrekte Code für das Tasenfeld wird über ein Array definiert
const int codelength = 4; //Array zum speichern von eingegebenen Zahlen
int code[codelength];  //Code hat 4 Ziffern
int index = 0;  //Arrayindex




//RFID
MFRC522 rfid(53,5); // RFID Empfänger benennen und Pins zuordnen
byte readcard[4]; //array mit ausgelesener RFID



LiquidCrystal lcd(8,7,48,46,49,47);   //LCD Pins


unsigned long v = 0;              //Zähler für unterstrich auf dem display
unsigned long blinkzeitms = 100; //Blinkzeit der roten LED bei tastendruck
unsigned long Zeitlastswitch = 0;
boolean strich = false;
bool status = false;            // Status bool ob tastenfeld freigegeben ist
bool richtigercode = false;       // boolean ob code richtig eingegeben wurde
int zeile=1;                    //Wechselt zwischen 0 und 1 für Emoji-Zeilensprung bei Melody
byte codes[][4] ={              //bekannte RFID Bibliothek
  {182, 73, 139, 142},
  {182, 73, 139, 141},
  {182, 73, 139, 143}
};
const byte numcodes = sizeof(codes) / sizeof(codes[0]); // Errechnet wie viele Codes in der Bibliothek sind

//Ultraschall
int SEND = 3; // Pin für Output Ultraschallsensor
int ECHO = 2; // Pin für input Ultraschallsensor
long distance = 0; // Speicherung der Entfernung
bool tueroffen = true;

//Servomotor
Servo Servomotor; 

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

  pinMode(SEND, OUTPUT); //Ultraschallsignal senden
  pinMode(ECHO, INPUT); //Ultraschallsignal empfangen

  Servomotor.attach(39); //Servomotor auf Pin 39 legen
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

void unterstrich(){     //boolean strich wechselt alle 600ms zwischen true und false
  if (millis() - v >= 600){
    strich = !strich;
    v = millis();
    for (int i = 0; i < index; i++)
    {
      lcd.setCursor(6+i,1);
      lcd.write("*");
    }  
    Serial.println(strich);
  }
}


void displayanzeige() {
    if (status == false){
      lcd.setCursor(0,0);     
      lcd.print("VAULT-TEC");
      lcd.setCursor(5,1);     
      lcd.print("LOCKED");

    }else{
      unterstrich();
      lcd.setCursor(0,0);     
      lcd.print("VAULT-TEC");
      lcd.setCursor(1,1);     
      lcd.print("Code:");
      if(index<4){
        lcd.setCursor((6+index),1);
        if(strich == true){
          lcd.print("_");
        }else{
          lcd.print(" ");
        }
      }
    }
}


//Methode zum Abgleich von zwei Arrays
bool compareArrays( byte array1[], byte array2[], byte length){
  for (byte i=0; i<length; i++){
    if(array1[i] != array2[i]){
      return false;
    }
  }
  return true;
}

bool compareArraysint( int array1[], int array2[], int length){ //Methode um integer arrays zu vergleichen
  for (int i=0; i<length; i++){
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

  if (status == false) {
    for (byte i=0; i<numcodes; i++){
      if (compareArrays(readcard, codes[i],4) == true){
        status = true;
        lcd.clear();
        tone(42,784,500);
        return;
      }
    }
    tonfalsch();
  }
}


//Methode, um Code Array zu printen
void showcode(int codearray[], int size){
  for (int x = 0; x < size; x++){
    Serial.print(codearray[x]);
  } 
}



//Ultraschallsensor Entfernungsmessung
long measuredistance() {

  digitalWrite(SEND, LOW); 
  delayMicroseconds(2);      //Sender kurz ausschalten zur Störungsvermeidung
  digitalWrite(SEND, HIGH);  //Ultraschallsignal für 10 Microsekunden senden
  delayMicroseconds(10);
  digitalWrite(SEND, LOW);

  long sonictime = pulseIn(ECHO, HIGH);  //Zeit bis das Ultraschallsignal zurückkommt
  return (sonictime/2)*0.0342;  //Formel zum berechnen des Abstands von Objekt zu Ultraschallsensor
}



// Läuft dauerhaft durch im späteren Code
void loop() {
  displayanzeige();

  getID();
  //getIDVergleich();
  
  char Taste = Tastenfeld.getKey(); //Mit Unter der Variablen pressedKey entspricht der gedrückten Taste
  
    //IF-Schleife um Array mit Code zu füllen
  if (status == true) {
  if (Taste) {
    if (isdigit(Taste)) {
      code[index] = (Taste - 48);  // Taste wird in int umgewandelt. char '0' = int 48
      index++;                     // Index geht eine Position weiter
      Serial.print(Taste);
      lcd.setCursor((5 + index), 1);
      lcd.write(Taste);
      v = millis();
      strich = false;
      if (index == codelength) {  // Wenn das Array gefüllt ist, wird der vollständige Code im Serial Monitor angezeigt
        Serial.println();
        Serial.print("Eingegebener Code: ");
        showcode(code, codelength);
        if (compareArraysint(code, codecorrect, 4) == true) {
          Serial.print(" korrekt");
          richtigercode = true;
        } else {
          Serial.print(" falsch");
        }
        Serial.println();
        lcd.clear();
        index = 0;  // nach Code-Eingabe wird Index auf 0 zurückgesetzt, um Array neu zu füllen
      }
    }
  }
}



//Distanzmessung
  distance = measuredistance(); //Aufrufen der Methode zur Distanzmessung
  
  if (distance <= 5)  //Abgleich des Türabstandes (kleiner als 5cm)
  {
    tueroffen = false;
    Serial.print("Tür ist geschlossen (");  //Ausgabe des Abstands auf dem Serial Monitor
    Serial.print(distance);
    Serial.print("cm) ");
    
  } else if (distance > 5){
      tueroffen = true;
      Serial.print("Tür ist offen. ");
  }
  
    

  if (Taste && Taste != 'A') {   //Bei allen Tasten außer 'A' wird die jeweilige Taste im Serial Monitor ausgegeben
    Zeitlastswitch = millis ();  //Bei Tastendruck wird zeit gemerkt
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
    lcd.clear();
  }


  if(status == false) {      //LED wechselt zwischen grün und rot abhängig vom Status
    digitalWrite(34, HIGH);
    digitalWrite(26, LOW);
  } else{
    digitalWrite(34, LOW);
    digitalWrite(26, HIGH);
  }
} 