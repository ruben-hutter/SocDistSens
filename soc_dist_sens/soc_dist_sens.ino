int trigger_pin = 2;
int echo_pin = 3;
int buzzer_pin = 10; 
int time;
int distance; 

void setup ( ) {

  Serial.begin (9600); 
  
  pinMode (trigger_pin, OUTPUT); 
  
  pinMode (echo_pin, INPUT);
  
  pinMode (buzzer_pin, OUTPUT);
  
  pinMode(13, OUTPUT);
}

void loop ( ) {

  digitalWrite (trigger_pin, HIGH);
  
  delayMicroseconds (10);
  
  digitalWrite (trigger_pin, LOW);
  
  time = pulseIn (echo_pin, HIGH);
  
  distance = (time * 0.034) / 2;
    
  if (distance <= 50) {

    Serial.print (" Distance= ");              

    Serial.println (distance);        

    digitalWrite(13, HIGH);

    delay (500);
  } else {
    Serial.print (" Distance= ");              

    Serial.println (distance);        

    digitalWrite(13, LOW);

    delay (500);        
  }
}
