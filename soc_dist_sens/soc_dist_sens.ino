/*
  LiquidCrystal Library

  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)
*/

#include <LiquidCrystal.h>

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

int distance;
boolean toClose;


/**
 * Reads data from ultrasonic sensor (time).
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
 * Converts ultrasonic distance to centimeters.
 */
int getUltrasonicDistance() {
  return 0.01723 * readUltrasonicDistance(TRIG_PIN, ECHO_PIN);
}


/**
 * Let LED blink and activate alarm
 * if distance to close.
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
 * Plays alarm sound
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
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}


void loop() {

  distance = getUltrasonicDistance();

  // print distance value
  lcd.setCursor(0, 0);
  lcd.print(distance);
  lcd.setCursor(4, 0);
  lcd.print("cm");
  lcd.setCursor(8, 0);
  lcd.print("|");

  if (distance < 200.00) {
    lcd.setCursor(0, 1);
    lcd.write("ABSTAND!|");
    toClose = true;
    setAlarm();
  } else {
    noTone(SPKR_PIN);           // turn Speaker off
    digitalWrite(LED_PIN, LOW); // turn LED off
    lcd.setCursor(0, 1);
    lcd.write("        |");
    toClose = false;
  }

  delay(100);
  lcd.clear();
}
  
