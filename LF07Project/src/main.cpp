#include <Arduino.h>

#define MOTOR1_SPEED 6
#define MOTOR2_SPEED 5
#define MOTOR1_INPUT1 9
#define MOTOR1_INPUT2 10
#define MOTOR2_INPUT1 7
#define MOTOR2_INPUT2 8
#define USS_TRIGGER 11
#define USS_ECHO 12

#define STATUS_LED 13

#define DISTANCE_WARNING 30
#define DISTANCE_STOP 10
#define DISTANCE_GOAGAIN 18

#define TURN_TIME 240
#define WAIT_AFTER_TURN 500
#define WAIT_AFTER_GO 250

int currentMotor1Speed = 0;
int currentMotor2Speed = 0;

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
void turn90DegreesLeft();
void goForwardsFor(int milliseconds);
void goBackwardsFor(int milliseconds);
void comeToAStop();
void turnLeftFor(int milliseconds);
void turnRightFor(int milliseconds);
void findFreeDirection();
void statusLed(int status);

void flow1();

void setup() {
  pinMode(STATUS_LED, OUTPUT);

  pinMode(MOTOR1_SPEED, OUTPUT);
  pinMode(MOTOR2_SPEED, OUTPUT);
  pinMode(MOTOR1_INPUT1, OUTPUT);
  pinMode(MOTOR1_INPUT2, OUTPUT);
  pinMode(MOTOR2_INPUT1, OUTPUT);
  pinMode(MOTOR2_INPUT2, OUTPUT);

  pinMode(USS_TRIGGER, OUTPUT);
  pinMode(USS_ECHO, INPUT);

  motor1Speed(255);
  motor2Speed(255);
}

void loop() {
  flow1();
}

// Ablaufversuch 1
void flow1() {
  float distance = getDistanceInCm();
  if (distance > DISTANCE_STOP) {
    motor1Forward();
    motor2Forward();
    motor1Speed(255);
    motor2Speed(255);
  } else {
    statusLed(1);
    comeToAStop();

    goBackwardsFor(500);
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

// Gibt die Entfernung in cm zurück
float getDistanceInCm() {
  digitalWrite(USS_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(USS_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(USS_TRIGGER, LOW);

  float duration = pulseIn(USS_ECHO, HIGH);
  float distance = duration * 0.034 / 2;

  return distance;
}


// Dreht sich um 90 Grad nach links
void turn90DegreesLeft() {
  motor1Backward();
  motor2Forward();
  motor1Speed(255);
  motor2Speed(255);
  delay(TURN_TIME);
  motor1Stop();
  motor2Stop();
}

// Fährt für eine bestimmte Zeit vorwärts
void goForwardsFor(int milliseconds) {
  motor1Forward();
  motor2Forward();
  motor1Speed(255);
  motor2Speed(255);
  delay(milliseconds);
  motor1Stop();
  motor2Stop();
}

// Fährt für eine bestimmte Zeit rückwärts
void goBackwardsFor(int milliseconds) {
  motor1Backward();
  motor2Backward();
  motor1Speed(255);
  motor2Speed(255);
  delay(milliseconds);
  motor1Stop();
  motor2Stop();
}

// Stoppt das Auto
void comeToAStop() {
  motor1Stop();
  motor2Stop();
}

// Dreht sich für eine bestimmte Zeit nach links
void turnLeftFor(int milliseconds) {
  motor1Speed(255);
  motor2Speed(255);
  motor1Backward();
  motor2Forward();
  delay(milliseconds);
  motor1Stop();
  motor2Stop();
}

// Dreht sich für eine bestimmte Zeit nach rechts
void turnRightFor(int milliseconds) {
  motor1Speed(160);
  motor2Speed(160);
  motor1Forward();
  motor2Backward();
  delay(milliseconds);
  motor1Stop();
  motor2Stop();
  motor1Speed(255);
  motor2Speed(255);
}

// Dreht sich nach links, bis es wieder genug Platz hat
void findFreeDirection() {
  bool foundFreeDirection = false;
  float distance = 0.0;
  int enoughSpaceCounter = 0;

  while (!foundFreeDirection) {
    distance = getDistanceInCm();

    if (distance > DISTANCE_GOAGAIN) {
      enoughSpaceCounter++;
    } else {
      enoughSpaceCounter = 0;
    }

    if (enoughSpaceCounter > 3) {
      foundFreeDirection = true;
    } else {
      turnLeftFor(250);
    }

    delay(200);
  }
}

void statusLed(int status) {
  digitalWrite(STATUS_LED, status);
}
