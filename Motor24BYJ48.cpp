#include "Arduino.h"
#include "Motor24BYJ48.h"
#include "Constants.h"

Motor24BYJ48::Motor24BYJ48(int pin1, int pin2, int pin3, int pin4, Punto esquina, int numMotor)
  : pin1(pin1), pin2(pin2), pin3(pin3), pin4(pin4), esquina(esquina) {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  indiceSecuenciaPasos = 0;
}



void Motor24BYJ48::stop() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
}


Punto Motor24BYJ48::getEsquina() const {
  return esquina;
}

float Motor24BYJ48::getPasos()
{
  return pasos;
}

void Motor24BYJ48::setPasos(float pasos)
{
    this->pasos = pasos * factorPasos;
}

float Motor24BYJ48::getIntervalo()
{
  return intervalo;
}

void Motor24BYJ48::setIntervalo(float intervalo)
{
    this->intervalo = intervalo;
}

void Motor24BYJ48::darPaso() {
  if (pasos == 0) {
    return; // No hay pasos que dar
  }
  // Determinar el sentido del giro
  int signo = pasos > 0 ? 1 : -1;

  // Actualizar el índice de la secuencia de pasos
  indiceSecuenciaPasos = (indiceSecuenciaPasos + signo) % 8;
  if (indiceSecuenciaPasos < 0) {
    indiceSecuenciaPasos += 8;
  }

  // Escribir la secuencia de pasos en los pines del motor
  digitalWrite(pin1, secuenciaPasos[indiceSecuenciaPasos][0]);
  digitalWrite(pin2, secuenciaPasos[indiceSecuenciaPasos][1]);
  digitalWrite(pin3, secuenciaPasos[indiceSecuenciaPasos][2]);
  digitalWrite(pin4, secuenciaPasos[indiceSecuenciaPasos][3]);

  // Decrementar el número de pasos a dar
  pasos -= signo;

}
