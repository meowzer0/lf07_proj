#include <Arduino.h>

int currentMotor1Speed = 0;
int currentMotor2Speed = 0;

#pragma region Konstanten
#define MOTOR1_SPEED 6
#define MOTOR2_SPEED 5
#define MOTOR1_INPUT1 2
#define MOTOR1_INPUT2 4
#define MOTOR2_INPUT1 7
#define MOTOR2_INPUT2 8
#define USS_TRIGGER 12
#define USS_ECHO 13

#define STATUS_LED_RED 10
#define STATUS_LED_GREEN 11
#define STATUS_LED_BLUE 9

#define BUZZER A0

#define MOTOR_DEFAULT_SPEED 170
#define MOTOR_SLOW_SPEED 110
#define MOTOR_FAST_SPEED 230

#define DISTANCE_STOP 15
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
float getDistanceInCm();
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
#pragma endregion

void setup() {
  pinMode(STATUS_LED_RED, OUTPUT);
  pinMode(STATUS_LED_GREEN, OUTPUT);
  pinMode(STATUS_LED_BLUE, OUTPUT);

  pinMode(MOTOR1_SPEED, OUTPUT);
  pinMode(MOTOR2_SPEED, OUTPUT);
  pinMode(MOTOR1_INPUT1, OUTPUT);
  pinMode(MOTOR1_INPUT2, OUTPUT);
  pinMode(MOTOR2_INPUT1, OUTPUT);
  pinMode(MOTOR2_INPUT2, OUTPUT);

  pinMode(USS_TRIGGER, OUTPUT);
  pinMode(USS_ECHO, INPUT);

  motor1Speed(MOTOR_DEFAULT_SPEED);
  motor2Speed(MOTOR_DEFAULT_SPEED);
}

void loop() {
  // flow1();
  motor1Speed(255);
  motor1Forward();
}

// Ablaufversuch 1
void flow1() {
  float distance = getDistanceInCm();
  if (distance > DISTANCE_STOP) {
    statusLed(2);
    autoForward();
    autoSpeed(MOTOR_DEFAULT_SPEED);
  } else {
    statusLed(1);
    comeToAStop();
    stopTone();

    goBackwardsFor(600, MOTOR_FAST_SPEED);
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
float getDistanceInCm() {
  digitalWrite(USS_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(USS_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(USS_TRIGGER, LOW);

  float duration = pulseIn(USS_ECHO, HIGH);
  float distance = ( duration / 29 ) / 2;

  return distance;
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
  float distance = 0.0;
  int enoughSpaceCounter = 0;

  // orangene status led
  statusLed(4);

  // zufällig entscheiden ob nach links oder rechts gedreht wird
  int direction = random(0, 2);
  void (*randomTurnFunction)(int, int);
  if (direction == 0) {
    randomTurnFunction = &turnLeftFor;
  } else {
    randomTurnFunction = &turnRightFor;
  } 

  while (!foundFreeDirection) {
    delay(50);
    distance = getDistanceInCm();

    if (distance > DISTANCE_GOAGAIN) {
      enoughSpaceCounter++;
    } else {
      enoughSpaceCounter = 0;
    }

    if (enoughSpaceCounter > 2) {
      foundFreeDirection = true;
    } else {
      randomTurnFunction(130, 255);
    }

    delay(100);
  }
}

// rgb led
// 0 = aus
// 1 = rot
// 2 = grün
// 3 = blau
// 4 = orange
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
    
  }
}

// buzzer
void playTone(int freq, int duration) {
  tone(BUZZER, freq);
  delay(duration);
  noTone(BUZZER);
}

void stopTone() {
  playTone(900, 200);
  delay(100);
  playTone(600, 150);
}
