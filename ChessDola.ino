/*
 Name:		ChessDola.ino
 Created:	24/05/2024 16:54:30
 Author:	Ofel
*/
#include "Motor24BYJ48.h"
#include "Constants.h"
#include <Arduino.h>
#include "Calculos.h"

Punto casillaOrigen;
Punto casillaDestino;
Punto origenCarte;
Punto destinoCarte;
bool capture;
char pieza;
bool debug = true;
bool asignado = false;


// Definir los tipos de piezas de ajedrez
enum Chesspieza { t, c, a, d, r, p };

float Deltas[4] = { 0, 0, 0, 0 };


// Definir los pines para cada motor
int motor1Pins[4] = { 8,9,10,11 };
int motor2Pins[4] = { 4,5,6,7 };
int motor3Pins[4] = { 22,24,26,28};
int motor4Pins[4] = { 40,42,44,46};

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


void asignarIntervalos(Motor24BYJ48 motor1, Motor24BYJ48 motor2, Motor24BYJ48 motor3, Motor24BYJ48 motor4) {

  int maxPasos = abs(motor1.getPasos());
  if (abs(motor2.getPasos()) > maxPasos) {
	maxPasos = abs(motor2.getPasos());
  }
  if (abs(motor3.getPasos()) > maxPasos){
	  maxPasos = abs(motor3.getPasos());
	  }
  if (abs(motor4.getPasos()) > maxPasos) {
	  maxPasos = abs(motor4.getPasos());
  }

  float factorSincro = maxPasos * MIN_INTERVALO;//...................
  Serial.print("Max pasos: ");
  Serial.println(maxPasos);
  Serial.print("Factor sincro: ");
  Serial.println(factorSincro);

  motor1.setIntervalo(abs(factorSincro/ motor1.getPasos()));

  motor2.setIntervalo(abs(factorSincro / motor2.getPasos()));
	
  motor3.setIntervalo(abs(factorSincro / motor3.getPasos()));

  motor4.setIntervalo(abs(factorSincro / motor4.getPasos()));

  Serial.print("Pasos motor 1: ");
  Serial.println(motor1.getPasos());
  Serial.print("Pasos motor 2: ");
  Serial.println(motor2.getPasos());
  Serial.print("Pasos motor 3: ");
  Serial.println(motor3.getPasos());
  Serial.print("Pasos motor 4: ");
  Serial.println(motor4.getPasos());
  Serial.println();
  Serial.print("Intervalo motor 1: ");	
  Serial.println(motor1.getIntervalo());
  Serial.print("Intervalo motor 2: ");
  Serial.println(motor2.getIntervalo());
  Serial.print("Intervalo motor 3: ");
  Serial.println(motor3.getIntervalo());
  Serial.print("Intervalo motor 4: ");
  Serial.println(motor4.getIntervalo());
}

void moverMotoresSinc(Motor24BYJ48& motor1, unsigned long intervalo1, Motor24BYJ48& motor2, unsigned long intervalo2, Motor24BYJ48& motor3, unsigned long intervalo3, Motor24BYJ48& motor4, unsigned long intervalo4) {
	static unsigned long ultimoPasoMotor1 = 0;
	static unsigned long ultimoPasoMotor2 = 0;
	static unsigned long ultimoPasoMotor3 = 0;
	static unsigned long ultimoPasoMotor4 = 0;

	unsigned long tiempoActual = micros();

	if (tiempoActual - ultimoPasoMotor1 >= intervalo1) {
		motor1.darPaso();
		ultimoPasoMotor1 = tiempoActual;
	}

	if (tiempoActual - ultimoPasoMotor2 >= intervalo2) {
		motor2.darPaso();
		ultimoPasoMotor2 = tiempoActual;
	}

	if (tiempoActual - ultimoPasoMotor3 >= intervalo3) {
		motor3.darPaso();
		ultimoPasoMotor3 = tiempoActual;
	}

	if (tiempoActual - ultimoPasoMotor4 >= intervalo4) {
		motor4.darPaso();
		ultimoPasoMotor4 = tiempoActual;
	}
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

		//Calcular la orientación
		Orientacion orientacion = calcularOrientacion(origenCarte, destinoCarte);
		//---------------------------------------------------------
		// Dentro de tu función loop, después de calcular la orientación
		switch (orientacion) {
		case orN:
			// Código para la orientación Norte
			//calcular diferencia de distancias Deltas
			Deltas[0] = calcularDiferenciaDistancias(motor1, origenCarte, destinoCarte);
			Deltas[1] = calcularDiferenciaDistancias(motor2, origenCarte, destinoCarte);
			Deltas[2] = calcularDiferenciaDistancias(motor3, origenCarte, destinoCarte);
			Deltas[3] = calcularDiferenciaDistancias(motor4, origenCarte, destinoCarte);

			//asignar pasos a cada motor
			motor1.setPasos(Deltas[0]);
			motor2.setPasos(Deltas[1]);
			motor3.setPasos(Deltas[2]);
			motor4.setPasos(Deltas[3]);

			//asignar intervalos proporcionales a los motores segun numero de pasos de cada uno
			asignarIntervalos(motor1, motor2, motor3, motor4);
			asignado = true;
			break;

		case orNE:
			// Código para la orientación Noreste
			break;
		case orE:
			// Código para la orientación Este
			break;
		case orSE:
			// Código para la orientación Sureste
			break;
		case orS:
			// Código para la orientación Sur
			break;
		case orSW:
			// Código para la orientación Suroeste
			break;
		case orW:
			// Código para la orientación Oeste
			break;
		case orNW:
			// Código para la orientación Noroeste
			break;
		default:
			// Código para cuando la orientación no coincide con ninguno de los casos anteriores
			break;
		}
	}
	if (asignado)
	{
	moverMotoresSinc(motor1, motor1.getIntervalo(), motor2, motor2.getIntervalo(), motor3, motor3.getIntervalo(), motor4, motor4.getIntervalo());

	}
	if (motor1.getPasos() == 0 && motor2.getPasos() == 0 && motor3.getPasos() == 0 && motor4.getPasos() == 0)
	{
		asignado = false;
		motor1.stop();
		motor2.stop();
		motor3.stop();
		motor4.stop();

	}

	

}

