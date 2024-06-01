#ifndef CALCULOS_H
#define CALCULOS_H

#include "Motor24BYJ48.h"

float calcularDistancia(Punto punto1, Punto punto2);
float calcularDiferenciaDistancias(Motor24BYJ48 motor, Punto origenCarte, Punto destinoCarte);
enum Orientacion { orN, orNE, orE, orSE, orS, orSW, orW, orNW };
Orientacion calcularOrientacion(Punto origenCarte, Punto destinoCarte);

#endif // CALCULOS_H

