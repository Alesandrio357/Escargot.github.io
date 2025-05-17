#include <Wire.h>
#include <Adafruit_VL6180X.h>

Adafruit_VL6180X vl = Adafruit_VL6180X();

const int sensorPins[] = {13, 14, 25, 26, 27, 32, 33, 35};
const int numSensors = 8;
int binaireWaarde = 0;
int sensorValues[8];
int sensorBinair = 0;

// Definieer motorpinnen (H-brug)
const int enA = 5;   // PWM voor snelheid motor A (pinSpeedLeft)
const int in3 = 16;   // Richting motor A
const int in4 = 17;
const int enB = 4;  // PWM voor snelheid motor B1
const int in1 = 19;   // Richting motor B
const int in2 = 18;

// Snelheden (PWM-waarden 0-255)
#define SPEED 145
#define SPEED_IN_TURN 80
#define FULL_TURN_SPEED 140
#define TURN_SPEED 160

//voor timers
bool actief = false;
bool actief2 = false;
bool actief3 = false;
bool actief4 = false;
bool actief5 = false;
bool actief6 = false;
int tijd1 = 0;
int tijd2 = 0;
int tijd3 = 0;
int tijd4 = 0;
int tijd5 = 0;
int tijd6 = 0;


char vorigeLijn;


void setup() {
  Serial.begin(115200); // Start serial communication

  // Initialize all sensor pins as inputs
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
  }
 
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.println("IR Sensor Array Test");
  Serial.println("--------------------");
}


void loop() {
    // INLEZEN AFSTANDSENSOR
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();
  if (status == VL6180X_ERROR_NONE) {
    Serial.print("Range: "); Serial.print(range); Serial.println("mm");
  }

  if (range < 100) {
        obstakel();
  }

  leesSensoren();
  char huidigeLijn = lijnAnalyse();
  maakManeuvre(huidigeLijn);
  Serial.println(); // New line after all sensors are printed
  Serial.println(); // New line after all sensors are printed
  //delay(100); // Small delay to make output readable
}


void obstakel(){
  if (!actief) {  // Start de timer
        tijd1 = millis();
        actief = true;
    }
  while(actief==true){
      draaiLinks();
      if (actief && millis() - tijd1 >= 550) {
        actief = false; // Reset voor volgende keer
    }}
  if (actief==false){
        tijd2=millis();
        actief2=true;
  }
  while(actief2==true){
      rechtDoor();
      if (actief2 && millis() - tijd2 >= 700) {
        actief2 = false; // Reset voor volgende keer
    }}
  if (actief2==false){
        tijd3=millis();
        actief3=true;
  }
  while(actief3==true){
      draaiRechts();
      if (actief3 && millis() - tijd3 >= 480) {
        actief3 = false; // Reset voor volgende keer
    }}
  if (actief3==false){
        tijd4=millis();
        actief4=true;
  }
  while(actief4==true){
      rechtDoor();
      if (actief4 && millis() - tijd4 >= 850) {
        actief4 = false; // Reset voor volgende keer
    }}
  if (actief4==false){
        tijd5=millis();
        actief5=true;
  }
  while(actief5==true){
      draaiRechts();
      if (actief5 && millis() - tijd5 >= 550) {
        actief5 = false; // Reset voor volgende keer
    }}
  if (actief5==false){
        tijd6=millis();
        actief6=true;
  }
  while(actief6==true){
      rechtDoor();
      if (actief6 && millis() - tijd6 >= 700) {
        actief6 = false; // Reset voor volgende keer
    }}    
}



void leesSensoren(){
  sensorBinair = 0;
    // Read all sensor values
  for (int i = 0; i < numSensors; i++) {
    int sensorValue = analogRead(sensorPins[i]);
   
    // Print sensor number and value
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(sensorValue);
   
    if (sensorValue < 4000){
      binaireWaarde = 1;
    }
    else{
      binaireWaarde = 0;
    }
    sensorValues[i] = binaireWaarde;

    // Add tab for alignment, except for the last sensor
    if (i < numSensors - 1) {
      Serial.print("\t");
    }
  }
  for (int i = 0; i < 8; i++) {
    sensorBinair |= (sensorValues[i] << (7 - i));  // spiegeling
    }
  Serial.println(sensorBinair, BIN);
}






char lijnAnalyse() {
  if (sensorBinair == 0b11100111 || sensorBinair == 0b11000111 || sensorBinair == 0b11100011 || sensorBinair == 0b11000011) {
    return 'S';  //rechtdoor
  } else if (sensorBinair == 0b11001111 || sensorBinair == 0b10001111 || sensorBinair == 0b10011111 || sensorBinair == 0b00111111 || sensorBinair == 0b01111111 || sensorBinair == 0b10000001 || sensorBinair == 0b10011001 || sensorBinair == 0b10011101|| sensorBinair == 0b10111101) {
    return 'l';  //afwijkingL
  } else if (sensorBinair == 0b11110011 || sensorBinair == 0b11110001 || sensorBinair == 0b11111001 || sensorBinair == 0b11111100 || sensorBinair == 0b11111110) {
    return 'r';  //afwijkingR
  } else if (sensorBinair == 0b00000111 || sensorBinair == 0b10000111 || sensorBinair == 0b00001111 || sensorBinair == 0b00011111 || sensorBinair == 0b00000011) {
    return 'L';  //splitsingL rechte hoek en schuine hoek
  } else if (sensorBinair == 0b11100000 || sensorBinair == 0b11100001 || sensorBinair == 0b11110000 || sensorBinair == 0b11111000 || sensorBinair == 0b11000000) {
    return 'R';  //splitsingR   rechte hoek en schuine hoek
  } else if (sensorBinair == 0b00000000) {
    return 'X';  //kruising
  } else if (sensorBinair == 0b11111111) {
    return 'N';  //niks
  }
  return 'O';  //ongeldig
}




void maakManeuvre(char huidigeLijn) {
  switch (huidigeLijn) {
    case 'S':
      rechtDoor();
      vorigeLijn = huidigeLijn;
      break;
    case 'l':
      bijsturingLinks();
      vorigeLijn = huidigeLijn;
      break;
    case 'L':
      draaiLinks();
      vorigeLijn = huidigeLijn;
      break;
    case 'r':
      bijsturingRechts();
      vorigeLijn = huidigeLijn;
      break;
    case 'R':
      draaiRechts();
      vorigeLijn = huidigeLijn;
      break;
    case 'X':
      kruising();
      vorigeLijn = huidigeLijn;
      break;
    case 'N':
      maakManeuvre(vorigeLijn);
      break;
    case 'O':
    default:
      Serial.println("ERROR");
      rechtDoor();
      break;
  }
}




void motorSnelheid(int leftSpeed, int rightSpeed) {
  Serial.print(leftSpeed);
  Serial.print(rightSpeed);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enA, leftSpeed);
  analogWrite(enB, rightSpeed);
}


void rechtDoor() {
  Serial.println("rechtdoor");
  motorSnelheid(SPEED, SPEED);
}


void stop() {
  Serial.println("rechtdoor");
  motorSnelheid(0, 0);
}


void draaiLinks() {
  Serial.println("draaiLinks");
  motorSnelheid(0, FULL_TURN_SPEED);
}


void bijsturingLinks(){
  Serial.println("bijsturingLinks");
  motorSnelheid(0, SPEED);
}


void draaiRechts() {
  Serial.println("draaiRechts");
  motorSnelheid(FULL_TURN_SPEED, 0);
}


void bijsturingRechts(){
  Serial.println("bijsturingRechts");
  motorSnelheid(SPEED, 0);
}


void kruising() {
  Serial.println("kruising");
  motorSnelheid(SPEED, SPEED);
}
