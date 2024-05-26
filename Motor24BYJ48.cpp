#include "Arduino.h"
#include "Motor24BYJ48.h"

Motor24BYJ48::Motor24BYJ48(int pin1, int pin2, int pin3, int pin4, Punto esquina)
  : pin1(pin1), pin2(pin2), pin3(pin3), pin4(pin4), esquina(esquina) {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
}

void Motor24BYJ48::setSpeed(int speed) {
  _speed = speed;
}

void Motor24BYJ48::step(int steps) {
  int seq[8][4] = {
      {HIGH, LOW, LOW, LOW},
      {HIGH, HIGH, LOW, LOW},
      {LOW, HIGH, LOW, LOW},
      {LOW, HIGH, HIGH, LOW},
      {LOW, LOW, HIGH, LOW},
      {LOW, LOW, HIGH, HIGH},
      {LOW, LOW, LOW, HIGH},
      {HIGH, LOW, LOW, HIGH}
  };

  if (steps > 0) {
    for (int i = 0; i < steps; i++) {
      for (int j = 0; j < 8; j++) {
        digitalWrite(pin1, seq[j][0]);
        digitalWrite(pin2, seq[j][1]);
        digitalWrite(pin3, seq[j][2]);
        digitalWrite(pin4, seq[j][3]);
        delay(_speed);
      }
    }
  }
  else {
    for (int i = 0; i < abs(steps); i++) {
      for (int j = 7; j >= 0; j--) {
        digitalWrite(pin1, seq[j][0]);
        digitalWrite(pin2, seq[j][1]);
        digitalWrite(pin3, seq[j][2]);
        digitalWrite(pin4, seq[j][3]);
        delay(_speed);
      }
    }
  }
}

void Motor24BYJ48::stop() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
}
