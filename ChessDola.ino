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
//..................................

void testCalcularDistancia() {
	Punto punto1 = { 0, 0 };
	Punto punto2 = { 100, 100 };
	float resultadoEsperado = 141.421356; // Reemplaza esto con el resultado esperado
	float resultado = calcularDistancia(punto1, punto2);
	Serial.print("Resultado: ");
	Serial.println(resultado);

	if (abs(resultado - resultadoEsperado) < 0.0001) {
		Serial.println("La prueba 1 de calcularDistancia pasa");
	}
	else {
		Serial.println("La prueba 1 de calcularDistancia FALLA");
	}

	punto1 = { 0, 0 };
	punto2 = { 200, 200 };
	resultadoEsperado = 282.8427; // Reemplaza esto con el resultado esperado
	resultado = calcularDistancia(punto1, punto2);
	if (abs(resultado - resultadoEsperado) < 0.0001) {
		Serial.println("La prueba 2 de calcularDistancia pasa");
	}
	else {
		Serial.println("La prueba 2 de calcularDistancia FALLA");
	}
}


void testCalcularDiferenciaDistancias() {
	Punto origen = { 35, 35 };
	Punto destino = { 245, 245 };
	float resultadoEsperado = 296.9848; // Reemplaza esto con el resultado esperado
	float resultado = calcularDiferenciaDistancias(motor1, origen, destino);
	Serial.print("Resultado: ");
	Serial.println(resultado);

	if (abs(resultado - resultadoEsperado) < 0.0001) {
		Serial.println("La prueba 1 de calcularDiferenciaDistancias pasa");
	}
	else {
		Serial.println("La prueba 1 de calcularDiferenciaDistancias FALLA");
	}

	origen = { 141, 141 };
	destino = { 141, 141 };
	resultadoEsperado = 0.0; // Reemplaza esto con el resultado esperado
	resultado = calcularDiferenciaDistancias(motor1, origen, destino);
	if (abs(resultado - resultadoEsperado) < 0.0001) {
		Serial.println("La prueba 2 de calcularDiferenciaDistancias pasa");
	}
	else {
		Serial.println("La prueba 2 de calcularDiferenciaDistancias FALLA");
	}
}

//..................................

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	inputString.reserve(200);
	//testCalcularDiferenciaDistancias();
	//testCalcularDistancia();
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
		if (debug)
		{
		Serial.print("Orientacion: ");
		Serial.println(orientacion);
		}
		// Calcular la diferencia de distancias
		float diferenciaDistancias1 = calcularDiferenciaDistancias(motor1, origenCarte, destinoCarte);
		float diferenciaDistancias2 = calcularDiferenciaDistancias(motor2, origenCarte, destinoCarte);
		float diferenciaDistancias3 = calcularDiferenciaDistancias(motor3, origenCarte, destinoCarte);
		float diferenciaDistancias4 = calcularDiferenciaDistancias(motor4, origenCarte, destinoCarte);
		if (debug)
		{
			Serial.print("Diferencia distancias 1: ");
			Serial.println(diferenciaDistancias1);
			Serial.print("Diferencia distancias 2: ");
			Serial.println(diferenciaDistancias2);
			Serial.print("Diferencia distancias 3: ");
			Serial.println(diferenciaDistancias3);
			Serial.print("Diferencia distancias 4: ");
			Serial.println(diferenciaDistancias4);

		}
	}
}

