/*
   Social Distancing Sensor
*/

#include <LiquidCrystal.h> // library for LCD-Display
#include <TFMPlus.h>       // library for Lidar: TFMini Plus Library
#include <MHZ.h>           // library for CO2: MHZ19B

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

// co2 sensor pins
#define CO2_IN 10 // pin for pwm reading

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
TFMPlus tfmP;     // TFMini Plus "object"
MHZ co2(CO2_IN, MHZ19B); // MH_Z19B "object"

int distance;
int ultrasonicDistance;
int lidarDistance;

int co2Level_pwm;
int temperature;

int16_t tfDist; // Distance to object in centimeters
int16_t tfFlux; // Strength or quality of return signal
int16_t tfTemp; // Internal temperature of Lidar sensor chip

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
   Get distance as an average value of lidar and ultrasonic sensor.
*/
int getDistance() {

  ultrasonicDistance = getUltrasonicDistance();
  lidarDistance = getLidarDistance();

  if (ultrasonicDistance < 30) {
    return ultrasonicDistance;
  } else if (lidarDistance > 400) {
    return lidarDistance;
  }
  return (lidarDistance + ultrasonicDistance) / 2;
}

/**
   Get 3 different values from co2 sensor:
   1) ppm_pwm
   2) temperature
*/
void getCo2Data() {
  co2Level_pwm = co2.readCO2PWM();
  temperature = co2.getLastTemperature(); // TODO check if correct after calibration
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

/**
   Print the co2 level data and temperature
   if the level is ok, or the "alarm message"
   if the level is to high, and you should aerate
   the room.
*/
void printCo2Data(bool toHigh) {
  if (toHigh) {
    lcd.setCursor(9, 0);
    lcd.print("AIR THE");
    lcd.setCursor(9, 1);
    lcd.print("ROOM!");
  } else {
    lcd.setCursor(9, 0);
    lcd.print(co2Level_pwm);
    lcd.setCursor(12, 0);
    lcd.print("ppm");
    lcd.setCursor(9, 1);
    lcd.print(temperature);
    lcd.setCursor(12, 1);
    lcd.print("C");
  }
}

/**
   Print distance if under 1200cm
   and ABSTAND if less than 200cm.
*/
void printDistance(int distance) {
  lcd.setCursor(0, 0);
  lcd.print(distance);
  lcd.setCursor(4, 0);
  lcd.print("cm");
  lcd.setCursor(8, 0);
  lcd.print("|");
  lcd.setCursor(0, 1);
  lcd.print("        |");


  if (distance < 200) {
    lcd.setCursor(0, 1);
    //lcd.print("ABSTAND!|");
    lcd.print("DISTANCE|");
    setAlarm();
  } else if (distance > 1200) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(">1200cm |");
  } else {
    noTone(SPKR_PIN);           // turn Speaker off
    digitalWrite(LED_PIN, LOW); // turn LED off
  }
}


void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  //Serial.begin(9600);
  Serial.begin(115200);
  delay(20);           // give port time to initialize
  tfmP.begin(&Serial); // initialize device library object and...
  pinMode(LED_PIN, OUTPUT);
  pinMode(CO2_IN, INPUT);
  delay(20);
  /*
    // let the co2 sensor preheat
    if (co2.isPreHeating()) {
      while (co2.isPreHeating()) {
        lcd.setCursor(0, 0);
        lcd.print("Preheating...");
        delay(100);
        lcd.clear();
      }
    }
  */

  // TODO check if calibration works, maybe delay needed
  //co2.setAutoCalibrate(true); TODO controll needed pin
}

void loop() {

  getCo2Data();
  distance = getDistance();

  // print distance value
  printDistance(distance);

  // print co2 value
  printCo2Data(co2Level_pwm < 1000);

  delay(100);
  lcd.clear();
}
