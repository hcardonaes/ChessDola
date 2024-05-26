#include "Calculos.h"
#include <math.h>

float calcularDistancia(Punto punto1, Punto punto2) {
  float dx = punto2.x - punto1.x;
  float dy = punto2.y - punto1.y;
  return sqrt(dx * dx + dy * dy);
}

float calcularDiferenciaDistancias(Motor24BYJ48 motor, Punto origenCarte, Punto destinoCarte) {
  float distanciaOrigen = calcularDistancia(motor.getEsquina(), origenCarte);
  float distanciaDestino = calcularDistancia(motor.getEsquina(), destinoCarte);
  return distanciaDestino - distanciaOrigen;
}
