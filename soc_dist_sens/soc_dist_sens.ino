/*
   Social Distancing Sensor
*/

#include <LiquidCrystal.h> // library for LCD-Display
#include <TFMPlus.h>       // library for Lidar: TFMini Plus Library

// speaker tunes
#define NOTE_C4  262
#define NOTE_G3  196

// ultrasonic sensor pins
#define TRIG_PIN  9
#define ECHO_PIN  7

// led pin
#define LED_PIN 14

// speaker pin
#define SPKR_PIN 8

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
TFMPlus tfmP;     // TFMini Plus "object"

int distance;
boolean toClose;

int16_t tfDist; // Distance to object in centimeters
int16_t tfFlux; // Strength or quality of return signal
int16_t tfTemp; // Internal temperature of Lidar sensor chip

int counter = 0; // counter for what?
char dist[11];   // ???

/**
   Reads data from ultrasonic sensor (time).
*/
long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);

  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}


/**
   Converts ultrasonic distance to centimeters.
*/
int getUltrasonicDistance() {
  return 0.01723 * readUltrasonicDistance(TRIG_PIN, ECHO_PIN);
}

/**
   Get the lidar distance in centimeters.
*/
int getLidarDistance() {
  // TODO possibly remove if statemant, if the getData() has a default "error" value
  if (tfmP.getData(tfDist, tfFlux, tfTemp)) {
    return tfDist;
  } else {
    return -1;
  }
}

/**
 * Get distance as an average value of lidar and ultrasonic sensor.
 */
int getDistance() {
  // TODO optimize this method, based on a scientific method
  // https://www.ncbi.nlm.nih.gov/pmc/articles/PMC8124335/

  return (getLidarDistance() + getUltrasonicDistance()) / 2;
}


/**
   Let LED blink and activate alarm
   if distance to close.
*/
void setAlarm() {
  digitalWrite(LED_PIN, HIGH);
  setTone(true);
  delay(1000);

  digitalWrite(LED_PIN, LOW);
  setTone(false);
  delay(1000);
}

/**
   Plays alarm sound
*/
void setTone(boolean ledIsOn) {
  if (ledIsOn) {
    noTone(SPKR_PIN);
    tone(SPKR_PIN, NOTE_C4);
  } else {
    noTone(SPKR_PIN);
    tone(SPKR_PIN, NOTE_G3);
  }
}


void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  Serial.begin(9600);
  //Serial.begin(115200);
  delay(20);           // give port time to initialize
  tfmP.begin(&Serial); // initialize device library object and...
  pinMode(LED_PIN, OUTPUT);
}


void loop() {

  distance = getDistance();

  // print distance value
  lcd.setCursor(0, 0);
  lcd.print(distance);
  lcd.setCursor(4, 0);
  lcd.print("cm");
  lcd.setCursor(8, 0);
  lcd.print("|");

  if (distance < 200.00) {
    lcd.setCursor(0, 1);
    lcd.print("ABSTAND!|");
    toClose = true;
    setAlarm();
  } else {
    noTone(SPKR_PIN);           // turn Speaker off
    digitalWrite(LED_PIN, LOW); // turn LED off
    lcd.setCursor(0, 1);
    lcd.print("        |");
    toClose = false;
  }

  delay(100);
  lcd.clear();
}
