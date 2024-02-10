const int soilMoisturePin = 2;
const int ledPin = 10;
const int Triger_Pin = 6; 
const int Echo_PinSet = 7;
const int PCINT_Pin = 4;

volatile bool sensorState = false;
volatile bool sensor2State = false;
volatile bool PCINT_State = false;
volatile bool Count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(3, INPUT_PULLUP);
  pinMode(soilMoisturePin, INPUT);
  pinMode(Triger_Pin, OUTPUT);
  pinMode(Echo_PinSet, INPUT);
  pinMode(PCINT_Pin, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);


  attachInterrupt(digitalPinToInterrupt(soilMoisturePin), handleInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), handleInterrupt2, RISING);
  

  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT20);
  
  cli(); // Disable interrupts
  TCCR1A = 0; // Reset Timer1 control registers
  TCCR1B = 0;
  TCNT1 = 0; // Reset Timer1 counter
  // Set compare match register for 1Hz (1 second) interrupt frequency
  OCR1A = 15624; // 16MHz/1024/1Hz - 1
  // Set CTC mode and prescaler 1024
  TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);
  // Enable Timer1 compare match A interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // En
  
}

void loop() {
  int tempValue = analogRead(A0);

  long T, D;
  
  digitalWrite(Triger_Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Triger_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Triger_Pin, LOW);
  
  T = pulseIn(Echo_PinSet, HIGH);
  D = T * 0.034 / 2;
  
  if (sensor2State) {
    Serial.println(tempValue);
    delay(2000);
    sensor2State = false;
  }
  
  if (sensorState) {
    Serial.println("WET");
    sensorState = false;
    digitalWrite(ledPin, LOW);
    delay(1000);
  } 
  else if (PCINT_State) {
    Serial.print("The Present Distance is: ");
    Serial.print(D);
    Serial.println(" cm");
    PCINT_State = false;
  }
  else {
    Serial.println("DRY");
  }

  delay(1000);
}

void handleInterrupt() {
  sensorState = true;
  digitalWrite(ledPin, HIGH);
}

void handleInterrupt2() {
  sensor2State = true;
}

ISR(PCINT2_vect) {
  PCINT_State = true;
}

ISR(TIMER1_COMPA_vect) {
  Count++;
  if (Count == 3) {
    digitalWrite(LED_BUILTIN, HIGH); // Turn on built-in LED
    delay(1000); // Wait for 1 second
    digitalWrite(LED_BUILTIN, LOW); // Turn off built-in LED
    Count = 0; // Reset cycle count
  }}
