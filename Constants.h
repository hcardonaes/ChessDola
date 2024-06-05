// constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

const int BOARD_SIZE = 560;
const float SQUARE_SIZE = BOARD_SIZE / 8.0; // Tamaño de cada casilla
const float factorPasos = 25; // Factor de conversión de mm a pasos
const float MIN_INTERVALO = 10000; // Reemplaza 1000 con el valor que desees
const int secuenciaPasos[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

#endif // CONSTANTS_H

