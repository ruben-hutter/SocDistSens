/*
   Social Distancing Sensor
*/

#include <LiquidCrystal.h> // library for LCD-Display
#include <TFMPlus.h>       // library for Lidar: TFMini Plus Library

//--------------------------------------------------------
#include <MHZ19.h>         // library for CO2: MHZ19B
#include <SoftwareSerial.h>
//--------------------------------------------------------

// Serial Boud rates
#define BAUDRATE_LIDAR 115200

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

//-----------------------------------------------
#define RX_PIN 6  // GREEN
#define TX_PIN 10 // BLUE
#define BAUDRATE_CO2 9600
MHZ19 myMHZ19;    // MH_Z19B "object"
SoftwareSerial mySerial(RX_PIN, TX_PIN);
//-----------------------------------------------

int distance;
int ultrasonicDistance;
int lidarDistance;

int co2Level;
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
  if (tfmP.getData(tfDist, tfFlux, tfTemp)) {
    return tfDist;
  }
  return -1;
}

/**
   Get distance as an average value of lidar and ultrasonic sensor.
*/
void getDistance() {

  ultrasonicDistance = getUltrasonicDistance();
  lidarDistance = getLidarDistance();

  if (ultrasonicDistance < 30) {
    distance = ultrasonicDistance;
  } else if (lidarDistance > 400) {
    distance = lidarDistance;
  } else {
    distance = (lidarDistance + ultrasonicDistance) / 2;
  }
}

/**
   Get 3 different values from co2 sensor:
   1) ppm_pwm
   2) temperature
*/
void getCo2Data() {
  co2Level = myMHZ19.getCO2();
  temperature = myMHZ19.getTemperature();
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
  toHigh = false;
  if (toHigh) {
    lcd.setCursor(9, 0);
    lcd.print("AIR THE");
    lcd.setCursor(9, 1);
    lcd.print("ROOM!");
  } else {
    lcd.setCursor(9, 0);
    lcd.print(co2Level);
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
  Serial.begin(BAUDRATE_LIDAR);
  delay(20);           // give port time to initialize
  tfmP.begin(&Serial); // initialize device library object and...
  pinMode(LED_PIN, OUTPUT);
  delay(20);
  //-----------------------------
  mySerial.begin(BAUDRATE_CO2);
  myMHZ19.begin(mySerial);
  myMHZ19.autoCalibration();
  //-----------------------------
}

void loop() {

  getCo2Data();

  // print co2 value
  printCo2Data(co2Level < 1000);

  getDistance();

  // print distance value
  printDistance(distance);

  delay(1000);
  lcd.clear();
}
