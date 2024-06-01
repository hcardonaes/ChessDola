#ifndef MOTOR24BYJ48_H
#define MOTOR24BYJ48_H

struct Punto {
  float x;
  float y;
};

class Motor24BYJ48 {
public:
  Motor24BYJ48(int pin1, int pin2, int pin3, int pin4, Punto esquina);
  void stop();
  Punto getEsquina() const;
  int getPasos();
  void setPasos(int pasos);
  int getIntervalo();
  void setIntervalo(int intervalo);
  void darPaso();
  int indiceSecuenciaPasos;


private:
  int pin1, pin2, pin3, pin4;
  Punto esquina;
  int pasos = 0;
  int intervalo = 1000;
};

#endif // MOTOR24BYJ48_H
