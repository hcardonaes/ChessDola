#ifndef MOTOR24BYJ48_H
#define MOTOR24BYJ48_H

struct Punto {
  float x;
  float y;
};

class Motor24BYJ48 {
public:
  Motor24BYJ48(int pin1, int pin2, int pin3, int pin4, Punto esquina);
  void setSpeed(int speed);
  void step(int steps);
  void stop();
  Punto getEsquina() const;

private:
  int pin1, pin2, pin3, pin4;
  Punto esquina;
  int _speed;
};

#endif // MOTOR24BYJ48_H
