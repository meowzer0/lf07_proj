// Abgabe LF07 Projektarbeit Patrick Pfaff 11IT2
// Code mit Historie auch in meinem GitHub-Repository: https://github.com/meowzer0/lf07_proj/blob/main/LF07Project/src/main.cpp

#include <Arduino.h>

int currentMotor1Speed = 0;
int currentMotor2Speed = 0;
int celebrationCounter = 0;

// buffer für linken und rechten sensor
float distances[2];
unsigned long millisAtMovementStart = 0;
bool movementStarted = false;
bool celebrationsDone = false;

#pragma region Konstanten
#define MOTOR1_SPEED 6
#define MOTOR2_SPEED 5
#define MOTOR1_INPUT1 2
#define MOTOR1_INPUT2 4
#define MOTOR2_INPUT1 7
#define MOTOR2_INPUT2 8
#define USS1_TRIGGER A3
#define USS1_ECHO A4
#define USS2_TRIGGER 12
#define USS2_ECHO 13

#define STATUS_LED_RED 10
#define STATUS_LED_GREEN 11
#define STATUS_LED_BLUE 9

#define BUZZER A0
#define START_BUTTON A5
#define STOP_BUTTON 3

#define MOTOR_DEFAULT_SPEED 170
#define MOTOR_SLOW_SPEED 110
#define MOTOR_FAST_SPEED 230

#define DISTANCE_STOP 20
#define DISTANCE_GOAGAIN 20

#define TURN_TIME 240
#define WAIT_AFTER_TURN 500
#define WAIT_AFTER_GO 250
#pragma endregion

#pragma region Funktionsdeklarationen
// Motor 1 = Links
// Motor 2 = Rechts
void motor1Forward();
void motor1Backward();
void motor1Stop();
void motor2Forward();
void motor2Backward();
void motor2Stop();
void motor1Speed(int speed);
void motor2Speed(int speed);
void updateDistances();
void goForwardsFor(int milliseconds, int desiredSpeed);
void goBackwardsFor(int milliseconds, int desiredSpeed);
void comeToAStop();
void turnLeftFor(int milliseconds, int desiredSpeed);
void turnRightFor(int milliseconds, int desiredSpeed);
void findFreeDirection();
void statusLed(int status);
void autoSpeed(int speed);
void returnToDefaultSpeed();
void playTone(int freq, int duration);
void autoForward();
void autoBackward();
void stopTone();
void flow1();
void waitForStartButton();
void stopAutoOnInterrupt();
void checkForCelebration();
void celebration(int level);
void level1celebration();
void level2celebration();
void level3celebration();
void playNokiaTone();
bool noObstacleInFront();
void autoSpeedCorrected(int speed);
#pragma endregion

void setup() {
  Serial.begin(9600);
  pinMode(STATUS_LED_RED, OUTPUT);
  pinMode(STATUS_LED_GREEN, OUTPUT);
  pinMode(STATUS_LED_BLUE, OUTPUT);

  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(STOP_BUTTON, INPUT_PULLUP);

  pinMode(MOTOR1_SPEED, OUTPUT);
  pinMode(MOTOR2_SPEED, OUTPUT);
  pinMode(MOTOR1_INPUT1, OUTPUT);
  pinMode(MOTOR1_INPUT2, OUTPUT);
  pinMode(MOTOR2_INPUT1, OUTPUT);
  pinMode(MOTOR2_INPUT2, OUTPUT);

  pinMode(USS1_TRIGGER, OUTPUT);
  pinMode(USS1_ECHO, INPUT);

  pinMode(USS2_TRIGGER, OUTPUT);
  pinMode(USS2_ECHO, INPUT);

  motor1Speed(MOTOR_DEFAULT_SPEED);
  motor2Speed(MOTOR_DEFAULT_SPEED);

  attachInterrupt(digitalPinToInterrupt(STOP_BUTTON), stopAutoOnInterrupt, LOW);

  waitForStartButton();
}

void loop() {
  flow1();
}

// Ablaufversuch 1
void flow1() {
  checkForCelebration();
  updateDistances();

  if (noObstacleInFront()) {
    statusLed(2);
    autoSpeedCorrected(MOTOR_DEFAULT_SPEED);
    autoForward();
  } else {
    statusLed(1);
    comeToAStop();
    goBackwardsFor(500, MOTOR_DEFAULT_SPEED);
    findFreeDirection();
    statusLed(0);
  }
}

// Fährt den Motor1 vorwärts
void motor1Forward() {
  digitalWrite(MOTOR1_INPUT1, HIGH);
  digitalWrite(MOTOR1_INPUT2, LOW);
}

// Fährt den Motor1 rückwärts
void motor1Backward() {
  digitalWrite(MOTOR1_INPUT1, LOW);
  digitalWrite(MOTOR1_INPUT2, HIGH);
}

// Stoppt den Motor1
void motor1Stop() {
  digitalWrite(MOTOR1_INPUT1, LOW);
  digitalWrite(MOTOR1_INPUT2, LOW);
}

// Fährt den Motor2 vorwärts
void motor2Forward() {
  digitalWrite(MOTOR2_INPUT1, HIGH);
  digitalWrite(MOTOR2_INPUT2, LOW);
}


// Fährt den Motor2 rückwärts
void motor2Backward() {
  digitalWrite(MOTOR2_INPUT1, LOW);
  digitalWrite(MOTOR2_INPUT2, HIGH);
}

// Stoppt den Motor2
void motor2Stop() {
  digitalWrite(MOTOR2_INPUT1, LOW);
  digitalWrite(MOTOR2_INPUT2, LOW);
}

void autoSpeed(int speed) {
  motor1Speed(speed);
  motor2Speed(speed);
}

// Fährt mit korrektur für den linksdrall
void autoSpeedCorrected(int speed) {
  int calcSpeedLeft = speed;
  int calcSpeedRight = speed - 20;

  if (calcSpeedRight < 0 ) calcSpeedRight = 0;
  motor1Speed(calcSpeedLeft);
  motor2Speed(calcSpeedRight);
}

// Fährt vorwärts
void autoForward() {
  motor1Forward();
  motor2Forward();
}

// Fährt rückwärts
void autoBackward() {
  motor1Backward();
  motor2Backward();
}

void motor1Speed(int speed) {
  if (speed != currentMotor1Speed) {
    currentMotor1Speed = speed;
    analogWrite(MOTOR1_SPEED, speed);
  }
}

void motor2Speed(int speed) {
  if (speed != currentMotor2Speed) {
    currentMotor2Speed = speed;
    analogWrite(MOTOR2_SPEED, speed);
  }
}

void returnToDefaultSpeed() {
  motor1Speed(MOTOR_DEFAULT_SPEED);
  motor2Speed(MOTOR_DEFAULT_SPEED);
}

// Gibt die Entfernung in cm zurück
void updateDistances() {
  int duration;
  float distance;

  // linker sensor
  digitalWrite(USS1_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(USS1_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(USS1_TRIGGER, LOW);
  duration = pulseIn(USS1_ECHO, HIGH);
  distance = ( duration / 29.0 ) / 2.0;

  distances[0] = distance;

  // rechter sensor
  digitalWrite(USS2_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(USS2_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(USS2_TRIGGER, LOW);

  duration = pulseIn(USS2_ECHO, HIGH);
  distance = ( duration / 29 ) / 2;

  distances[1] = distance;
}

// Fährt für eine bestimmte Zeit vorwärts
void goForwardsFor(int milliseconds, int desiredSpeed) {
  autoSpeed(desiredSpeed);
  autoForward();
  delay(milliseconds);
  comeToAStop();

  returnToDefaultSpeed();
}

// Fährt für eine bestimmte Zeit rückwärts
void goBackwardsFor(int milliseconds, int desiredSpeed) {
  autoSpeed(desiredSpeed);
  autoBackward();
  delay(milliseconds);
  comeToAStop();

  returnToDefaultSpeed();
}

// Stoppt das Auto
void comeToAStop() {
  int halfSpeed = MOTOR_DEFAULT_SPEED / 2;
  autoSpeed(halfSpeed);
  delay(200);
  motor1Stop();
  motor2Stop();
}

// Dreht sich für eine bestimmte Zeit nach links
void turnLeftFor(int milliseconds, int desiredSpeed) {
  autoSpeed(desiredSpeed);
  motor1Backward();
  motor2Forward();
  delay(milliseconds);
  motor1Stop();
  motor2Stop();

  returnToDefaultSpeed();
}

// Dreht sich für eine bestimmte Zeit nach rechts
void turnRightFor(int milliseconds, int desiredSpeed) {
  autoSpeed(desiredSpeed);
  motor1Forward();
  motor2Backward();
  delay(milliseconds);
  motor1Stop();
  motor2Stop();

  returnToDefaultSpeed();
}

// Dreht sich nach links, bis es wieder genug Platz hat
void findFreeDirection() {
  bool foundFreeDirection = false;
  int enoughSpaceCounter = 0;

  // orangene status led
  statusLed(2);

  // zufällig entscheiden ob nach links oder rechts gedreht wird
  int direction = random(0, 2);
  void (*randomTurnFunction)(int, int);
  if (direction == 0) {
    randomTurnFunction = &turnLeftFor;
  } else {
    randomTurnFunction = &turnRightFor;
  } 

  while (!foundFreeDirection) {
    randomTurnFunction(200, MOTOR_DEFAULT_SPEED);
    delay(150);    
    if (noObstacleInFront()) {
      enoughSpaceCounter++;
    } else {
      enoughSpaceCounter = 0;
    }
    if (enoughSpaceCounter > 1) foundFreeDirection = true;
  }

  statusLed(0);
}

// rgb led
// 0 = aus
// 1 = rot
// 2 = grün
// 3 = blau
// 4 = orange
// 5 = lila
// 6 = gelb
// 7 = weiß
// 8 = türkis
void statusLed(int status) {
  #define BRIGHT 255
  #define MEDIUM 150
  #define DIM 100
  
  switch (status) {
    case 0:
      analogWrite(STATUS_LED_RED, LOW);
      analogWrite(STATUS_LED_GREEN, LOW);
      analogWrite(STATUS_LED_BLUE, LOW);
      break;

    case 1:
      analogWrite(STATUS_LED_RED, BRIGHT);
      analogWrite(STATUS_LED_GREEN, LOW);
      analogWrite(STATUS_LED_BLUE, LOW);
      break;

    case 2:
      analogWrite(STATUS_LED_RED, LOW);
      analogWrite(STATUS_LED_GREEN, BRIGHT);
      analogWrite(STATUS_LED_BLUE, LOW);
      break;

    case 3:
      analogWrite(STATUS_LED_RED, LOW);
      analogWrite(STATUS_LED_GREEN, LOW);
      analogWrite(STATUS_LED_BLUE, BRIGHT);
      break;

    case 4:
      analogWrite(STATUS_LED_RED, MEDIUM);
      analogWrite(STATUS_LED_GREEN, DIM);
      analogWrite(STATUS_LED_BLUE, LOW);
      break;

    case 5:
      analogWrite(STATUS_LED_RED, BRIGHT);
      analogWrite(STATUS_LED_GREEN, LOW);
      analogWrite(STATUS_LED_BLUE, MEDIUM);
      break;

    case 6:
      analogWrite(STATUS_LED_RED, BRIGHT);
      analogWrite(STATUS_LED_GREEN, BRIGHT);
      analogWrite(STATUS_LED_BLUE, LOW);
      break;

    case 7:
      analogWrite(STATUS_LED_RED, BRIGHT);
      analogWrite(STATUS_LED_GREEN, BRIGHT);
      analogWrite(STATUS_LED_BLUE, BRIGHT);
      break;

    case 8:
      analogWrite(STATUS_LED_RED, LOW);
      analogWrite(STATUS_LED_GREEN, BRIGHT);
      analogWrite(STATUS_LED_BLUE, BRIGHT);
      break;
  }
}

// buzzer
void playTone(int freq, int duration) {
  tone(BUZZER, freq);
  delay(duration);
  noTone(BUZZER);
}

// ton, wenn das auto stehen bleibt
void stopTone() {
  playTone(900, 200);
  delay(100);
  playTone(600, 150);
}

void waitForStartButton() {
  bool startButtonPressed = false;

  // lila status led = warte auf start button
  statusLed(5);
  while (!startButtonPressed) {
    if (digitalRead(START_BUTTON) == LOW) {
      startButtonPressed = true;
    }
  }

  // Damit die Zeit, die für die celebration gebraucht wird, nicht mitgezählt wird wenn das auto noch nicht fährt
  if (!movementStarted)
  {
    movementStarted = true;
    millisAtMovementStart = millis();
  }

  statusLed(0);
}

void stopAutoOnInterrupt() {
  comeToAStop();
  stopTone();
  statusLed(0);

  waitForStartButton();
}

void checkForCelebration() {
  if (celebrationsDone) return;

  unsigned long timeElapsed = millis();
  Serial.print("Time since arduino started: ");
  Serial.println(timeElapsed);
  Serial.print("Time since movement started: ");
  Serial.println(timeElapsed - millisAtMovementStart);

  // Zeit vor dem Fahren soll nicht mitgezählt werden
  if (timeElapsed > 15000  + millisAtMovementStart && celebrationCounter < 1) {
    celebration(0);
    celebrationCounter++;
  }
  else if (timeElapsed > 25000  + millisAtMovementStart && celebrationCounter < 2) {
    celebration(1);
    celebrationCounter++;
  } 
  else if (timeElapsed > 35000 + millisAtMovementStart && celebrationCounter < 3) {
    celebration(2);
    celebrationCounter++;
  }
}

void celebration(int level) {
  switch (level) {
    case 0:
      level1celebration();
      break;
    case 1:
      level2celebration();
      break;
    case 2:
      level3celebration();
      break;
  }
}

// feuerwerk mit led
void level1celebration() {
  playTone(900, 200);
  comeToAStop();
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < 9; i++) {
      statusLed(i);
      delay(150);
    }
  }
}

// feuerwerk mit led und ton
void level2celebration() {
  comeToAStop();
  playTone(900, 200);
  playNokiaTone();
}

// feuerwerk mit led und ton und tanzen
void level3celebration() {
  comeToAStop();
  playTone(900, 200);
  turnLeftFor(2000, 255);
  turnRightFor(2000, 255);
  for (int i = 0; i < 9; i++) {
      statusLed(i);
      delay(100);
  }
  for (int i = 0; i < 4; i++) {
    turnLeftFor(800, 255);
    statusLed(random(0, 8));
    delay(200);
  }
  for (int i = 0; i < 4; i++) {
    turnRightFor(800, 255);
    statusLed(random(0, 8));
    delay(200);
  }
  turnLeftFor(2000, 255);
    for (int i = 0; i < 9; i++) {
      statusLed(i);
      delay(100);
  }

  celebrationsDone = true;
}

bool noObstacleInFront() {
  bool obstacleInFront = false;
  updateDistances();

  if (distances[0] < DISTANCE_STOP || distances[1] < DISTANCE_STOP) {
    obstacleInFront = true;
  }

  return !obstacleInFront;
}

// Noten kommen aus dem Internet:
// https://github.com/robsoncouto/arduino-songs/tree/master/nokia
void playNokiaTone() {
  /* 
  Nokia Tune
  Connect a piezo buzzer or speaker to pin 11 or select a new pin.
  More songs available at https://github.com/robsoncouto/arduino-songs                                            
                                              
                                              Robson Couto, 2019
  */
  #define NOTE_B0  31
  #define NOTE_C1  33
  #define NOTE_CS1 35
  #define NOTE_D1  37
  #define NOTE_DS1 39
  #define NOTE_E1  41
  #define NOTE_F1  44
  #define NOTE_FS1 46
  #define NOTE_G1  49
  #define NOTE_GS1 52
  #define NOTE_A1  55
  #define NOTE_AS1 58
  #define NOTE_B1  62
  #define NOTE_C2  65
  #define NOTE_CS2 69
  #define NOTE_D2  73
  #define NOTE_DS2 78
  #define NOTE_E2  82
  #define NOTE_F2  87
  #define NOTE_FS2 93
  #define NOTE_G2  98
  #define NOTE_GS2 104
  #define NOTE_A2  110
  #define NOTE_AS2 117
  #define NOTE_B2  123
  #define NOTE_C3  131
  #define NOTE_CS3 139
  #define NOTE_D3  147
  #define NOTE_DS3 156
  #define NOTE_E3  165
  #define NOTE_F3  175
  #define NOTE_FS3 185
  #define NOTE_G3  196
  #define NOTE_GS3 208
  #define NOTE_A3  220
  #define NOTE_AS3 233
  #define NOTE_B3  247
  #define NOTE_C4  262
  #define NOTE_CS4 277
  #define NOTE_D4  294
  #define NOTE_DS4 311
  #define NOTE_E4  330
  #define NOTE_F4  349
  #define NOTE_FS4 370
  #define NOTE_G4  392
  #define NOTE_GS4 415
  #define NOTE_A4  440
  #define NOTE_AS4 466
  #define NOTE_B4  494
  #define NOTE_C5  523
  #define NOTE_CS5 554
  #define NOTE_D5  587
  #define NOTE_DS5 622
  #define NOTE_E5  659
  #define NOTE_F5  698
  #define NOTE_FS5 740
  #define NOTE_G5  784
  #define NOTE_GS5 831
  #define NOTE_A5  880
  #define NOTE_AS5 932
  #define NOTE_B5  988
  #define NOTE_C6  1047
  #define NOTE_CS6 1109
  #define NOTE_D6  1175
  #define NOTE_DS6 1245
  #define NOTE_E6  1319
  #define NOTE_F6  1397
  #define NOTE_FS6 1480
  #define NOTE_G6  1568
  #define NOTE_GS6 1661
  #define NOTE_A6  1760
  #define NOTE_AS6 1865
  #define NOTE_B6  1976
  #define NOTE_C7  2093
  #define NOTE_CS7 2217
  #define NOTE_D7  2349
  #define NOTE_DS7 2489
  #define NOTE_E7  2637
  #define NOTE_F7  2794
  #define NOTE_FS7 2960
  #define NOTE_G7  3136
  #define NOTE_GS7 3322
  #define NOTE_A7  3520
  #define NOTE_AS7 3729
  #define NOTE_B7  3951
  #define NOTE_C8  4186
  #define NOTE_CS8 4435
  #define NOTE_D8  4699
  #define NOTE_DS8 4978
  #define REST      0


  // change this to make the song slower or faster
  int tempo = 180;

  // change this to whichever pin you want to use
  int buzzer = BUZZER;

  // notes of the moledy followed by the duration.
  // a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
  // !!negative numbers are used to represent dotted notes,
  // so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
  int melody[] = {

    // Nokia Ringtone 
    // Score available at https://musescore.com/user/29944637/scores/5266155
    
    NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4, 
    NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4, 
    NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
    NOTE_A4, 2, 
  };

  // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
  // there are two values per note (pitch and duration), so for each note there are four bytes
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;

  int divider = 0, noteDuration = 0;

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzer);
  }
}
