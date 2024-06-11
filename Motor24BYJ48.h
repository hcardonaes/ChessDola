#ifndef MOTOR24BYJ48_H
#define MOTOR24BYJ48_H

struct Punto {
  float x;
  float y;
};

class Motor24BYJ48 {
public:
  Motor24BYJ48(int pin1, int pin2, int pin3, int pin4, Punto esquina, int numMotor);
  void stop();
  Punto getEsquina()const;
  float getPasos();
  void setPasos(float pasos);
  float getIntervalo();
  void setIntervalo(long intervalo);
  void darPaso();
  int indiceSecuenciaPasos;
  int numMotor;

private:
  int pin1, pin2, pin3, pin4;
  Punto esquina;
  int pasos = 0;
  long intervalo = 1000;
};

#endif // MOTOR24BYJ48_H
