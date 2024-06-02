#include "Calculos.h"
#include <math.h>
#include <Arduino.h>

float calcularDistancia(Punto punto1, Punto punto2) {
  float dx = punto2.x - punto1.x;
  float dy = punto2.y - punto1.y;
  return sqrt(dx * dx + dy * dy);
}

float calcularDiferenciaDistancias(Motor24BYJ48 motor, Punto origenCarte, Punto destinoCarte) {
  float distanciaOrigen = calcularDistancia(motor.getEsquina(), origenCarte);
  ////Serial.print("Distancia Origen: ");
  ////Serial.println(distanciaOrigen);

  float distanciaDestino = calcularDistancia(motor.getEsquina(), destinoCarte);
  ////Serial.print("Distancia Destino: ");
  ////Serial.println(distanciaDestino);

  return  distanciaOrigen-distanciaDestino;
}

Orientacion calcularOrientacion(Punto origenCarte, Punto destinoCarte)
{
  float dx = destinoCarte.x - origenCarte.x;
  float dy = destinoCarte.y - origenCarte.y;
  float angle = atan2(dy, dx) * 180 / 3.14159;
  if (angle < 0) {
    angle += 360;
  }
  if (angle >= 337.5 || angle < 22.5) {
    return orE;
  }
  else if (angle >= 22.5 && angle < 67.5) {
    return orNE;
  }
  else if (angle >= 67.5 && angle < 112.5) {
    return orN;
  }
  else if (angle >= 112.5 && angle < 157.5) {
    return orNW;
  }
  else if (angle >= 157.5 && angle < 202.5) {
    return orW;
  }
  else if (angle >= 202.5 && angle < 247.5) {
    return orSW;
  }
  else if (angle >= 247.5 && angle < 292.5) {
    return orS;
  }
  else {
    return orSE;
  }
}

bool relativoOrto(Punto origenCarte, Punto destinoCarte)
{
  float dx = destinoCarte.x - origenCarte.x;
  float dy = destinoCarte.y - origenCarte.y;
  if (dx == 0 || dy == 0) {
    return true;
    Serial.println("Es ortogonal");
  }
  if (abs(dx) == abs(dy)) {
    Serial.println("Es diagonal");
    return false;
  }
}

