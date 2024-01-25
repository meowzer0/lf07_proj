#include <Arduino.h>

#define MOTOR1_SPEED 5
#define MOTOR2_SPEED 6
#define MOTOR1_INPUT1 7
#define MOTOR1_INPUT2 8
#define MOTOR2_INPUT1 9
#define MOTOR2_INPUT2 10

#define LED_SIGNALRED 2
#define LED_SIGNALYELLOW 3
#define LED_SIGNALGREEN 4

#define USS_TRIGGER 11
#define USS_ECHO 12

#define DISTANCE_WARNING 30
#define DISTANCE_STOP 15

#define TURN_TIME 400

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
void setSignalRed();
void setSignalYellow();
void setSignalGreen();
float getDistanceInCm();
void turn90DegreesLeft();

void setup() {
  Serial.begin(9600);

  pinMode(LED_SIGNALRED, OUTPUT);
  pinMode(LED_SIGNALYELLOW, OUTPUT);
  pinMode(LED_SIGNALGREEN, OUTPUT);

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
  float distance = getDistanceInCm();
  Serial.println(distance);
  if (distance > DISTANCE_STOP) {
    motor1Forward();
    motor2Forward();
    motor1Speed(255);
    motor2Speed(255);
  }
  else
  {
    motor1Stop();
    motor2Stop();

    turn90DegreesLeft();
  }

  // Ampel
  if (distance > DISTANCE_WARNING)
  {
    setSignalGreen();
  }
  else if (distance > DISTANCE_STOP)
  {
    setSignalYellow();
  }
  else
  {
    setSignalRed();
  }

  delay(50);
}

void motor1Forward() {
  digitalWrite(MOTOR1_INPUT1, HIGH);
  digitalWrite(MOTOR1_INPUT2, LOW);
}

void motor1Backward() {
  digitalWrite(MOTOR1_INPUT1, LOW);
  digitalWrite(MOTOR1_INPUT2, HIGH);
}

void motor1Stop() {
  digitalWrite(MOTOR1_INPUT1, LOW);
  digitalWrite(MOTOR1_INPUT2, LOW);
}

void motor2Forward() {
  digitalWrite(MOTOR2_INPUT1, HIGH);
  digitalWrite(MOTOR2_INPUT2, LOW);
}

void motor2Backward() {
  digitalWrite(MOTOR2_INPUT1, LOW);
  digitalWrite(MOTOR2_INPUT2, HIGH);
}

void motor2Stop() {
  digitalWrite(MOTOR2_INPUT1, LOW);
  digitalWrite(MOTOR2_INPUT2, LOW);
}

void motor1Speed(int speed) {
  analogWrite(MOTOR1_SPEED, speed);
}

void motor2Speed(int speed) {
  analogWrite(MOTOR2_SPEED, speed);
}

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

void setSignalRed() {
  digitalWrite(LED_SIGNALRED, HIGH);
  digitalWrite(LED_SIGNALYELLOW, LOW);
  digitalWrite(LED_SIGNALGREEN, LOW);
}

void setSignalYellow() {
  digitalWrite(LED_SIGNALRED, LOW);
  digitalWrite(LED_SIGNALYELLOW, HIGH);
  digitalWrite(LED_SIGNALGREEN, LOW);
}

void setSignalGreen() {
  digitalWrite(LED_SIGNALRED, LOW);
  digitalWrite(LED_SIGNALYELLOW, LOW);
  digitalWrite(LED_SIGNALGREEN, HIGH);
}

void turn90DegreesLeft() {
  motor1Backward();
  motor2Forward();
  motor1Speed(255);
  motor2Speed(255);
  delay(TURN_TIME);
  motor1Stop();
  motor2Stop();
  delay(1000);
}
