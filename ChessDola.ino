/*
 Name:		ChessDola.ino
 Created:	24/05/2024 16:54:30
 Author:	Ofel
*/
#include "Motor24BYJ48.h"
#include "Constants.h"
#include <Arduino.h>

Punto casillaOrigen;
Punto casillaDestino;
Punto origenCarte;
Punto destinoCarte;

bool capture;
char pieza;
bool debug = true;

// Definir los tipos de piezas de ajedrez
enum Chesspieza { t, c, a, d, r, p };

// Definir los pines para cada motor
int motor1Pins[4] = { 2, 3, 4, 5 };
int motor2Pins[4] = { 6, 7, 8, 9 };
int motor3Pins[4] = { 10, 11, 12, 13 };
int motor4Pins[4] = { 14, 15, 16, 17 };

Punto esquina1 = {0, 0};
Punto esquina2 = {0, BOARD_SIZE};
Punto esquina3 = {BOARD_SIZE, BOARD_SIZE};
Punto esquina4 = {BOARD_SIZE, 0};


// Crear las instancias de los motores
Motor24BYJ48 motor1(motor1Pins[0], motor1Pins[1], motor1Pins[2], motor1Pins[3], esquina1);
Motor24BYJ48 motor2(motor2Pins[0], motor2Pins[1], motor2Pins[2], motor2Pins[3], esquina2);
Motor24BYJ48 motor3(motor3Pins[0], motor3Pins[1], motor3Pins[2], motor3Pins[3], esquina3);
Motor24BYJ48 motor4(motor4Pins[0], motor4Pins[1], motor4Pins[2], motor4Pins[3], esquina4);

String inputString = "";         // Un String para guardar los datos entrantes
bool stringComplete = false;  // Si el string está completo

Punto notacionToCasilla(char column, char row) {
	Punto coordenadas;
	coordenadas.x = column - 'a' + 1;
	coordenadas.y = row - '1' + 1;
	return coordenadas;
}

Punto casillaToCoordenadas(Punto casilla) {
  Punto coordenadas;
  coordenadas.x = (casilla.x - 1) * SQUARE_SIZE + SQUARE_SIZE/2;
  coordenadas.y = (casilla.y - 1) * SQUARE_SIZE + SQUARE_SIZE/2;
  return coordenadas;
}


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	inputString.reserve(200);
}

void loop() {
    // Leer los datos entrantes

    if (Serial.available()) {
        inputString = Serial.readStringUntil('\n');
        stringComplete = true;
    }


	if (stringComplete) {
		// Parsear el string
		// Ejemplo de string: "td2-d4"     

		pieza = inputString[0];
		casillaOrigen = notacionToCasilla(inputString[1], inputString[2]);
		origenCarte = casillaToCoordenadas(casillaOrigen);
		casillaDestino = notacionToCasilla(inputString[4], inputString[5]);
		destinoCarte = casillaToCoordenadas(casillaDestino);
		capture = (inputString[3] == 'x');
		if (debug)
		{
		Serial.print("input: ");
		Serial.println(inputString);
		Serial.print("Origen x: ");
		Serial.println(casillaOrigen.x);
		Serial.print("Origen y: ");
		Serial.println(casillaOrigen.y);
		Serial.print("Origen mm x: ");
		Serial.println(origenCarte.x);
		Serial.print("Origen mm y: ");
		Serial.println(origenCarte.y);
		Serial.print("Destino x: ");
		Serial.println(casillaDestino.x);
		Serial.print("Destino y: ");
		Serial.println(casillaDestino.y);
		Serial.print("Destino x mm: ");
		Serial.println(destinoCarte.x);
		Serial.print("Destino y mm: ");
		Serial.println(destinoCarte.y);
		Serial.print("Capture: ");
		Serial.println(capture);
		Serial.print("pieza: ");
		Serial.println(pieza);
		}
		inputString = "";
		stringComplete = false;
	}
}

