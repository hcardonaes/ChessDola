/*
 Name:		ChessDola.ino
 Created:	24/05/2024 16:54:30
 Author:	Ofel
*/

#include "Motor24BYJ48.h"
#include "Constants.h"
#include <Arduino.h>
#include <math.h>
#include <string.h>

Punto casillaOrigen;
Punto casillaDestino;
Punto origenCarte;
Punto destinoCarte;
bool capture;
char pieza;
bool debug = false;
bool asignado = false;

struct Proyecciones {
	Punto motor1;
	Punto motor2;
};

// Definir los tipos de piezas de ajedrez
enum Chesspieza { t, c, a, d, r, p };

float Deltas[4] = { 0, 0, 0, 0 };

// Definir los pines para cada motor
int motor1Pins[4] = { 4,5,6,7 };
int motor2Pins[4] = {8,9,10,11};
int motor3Pins[4] = { 40,42,44,46};
int motor4Pins[4] = { 22,24,26,28};

Punto esquina1 = {0, 0};
Punto esquina2 = {0, BOARD_SIZE};
Punto esquina3 = {BOARD_SIZE, BOARD_SIZE};
Punto esquina4 = {BOARD_SIZE, 0};

Punto casillaAnterior = { d, 4 };

// Crear las instancias de los motores
Motor24BYJ48 motor1(motor1Pins[0], motor1Pins[1], motor1Pins[2], motor1Pins[3], esquina1, 1);
Motor24BYJ48 motor2(motor2Pins[0], motor2Pins[1], motor2Pins[2], motor2Pins[3], esquina2, 2);
Motor24BYJ48 motor3(motor3Pins[0], motor3Pins[1], motor3Pins[2], motor3Pins[3], esquina3, 3);
Motor24BYJ48 motor4(motor4Pins[0], motor4Pins[1], motor4Pins[2], motor4Pins[3], esquina4, 4);

String inputString = "";         // Un String para guardar los datos entrantes
bool stringComplete = false;  // Si el string está completo

Punto notacionChessToOrdinalCasilla(char column, char row) {
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

Proyecciones calcularProyecciones(Punto origen, Punto destino, Punto motor1, Punto motor2) {
	// Calcular la pendiente de la línea
	float m = (destino.y - origen.y) / (destino.x - origen.x);

	// Calcular el término independiente de la línea
	float b = origen.y - m * origen.x;

	// Calcular las proyecciones de los motores sobre la línea
	Proyecciones proyecciones;
	proyecciones.motor1.x = (m * motor1.y + motor1.x - m * b) / (m * m + 1);
	proyecciones.motor1.y = (m * m * motor1.y + m * motor1.x + b) / (m * m + 1);
	proyecciones.motor2.x = (m * motor2.y + motor2.x - m * b) / (m * m + 1);
	proyecciones.motor2.y = (m * m * motor2.y + m * motor2.x + b) / (m * m + 1);

	return proyecciones;
	}

bool estaDentroDelSegmento(Punto origen, Punto destino, Punto proyeccion) {
	// Calcular los rangos de las coordenadas x e y
	float minX = min(origen.x, destino.x);
	float maxX = max(origen.x, destino.x);
	float minY = min(origen.y, destino.y);
	float maxY = max(origen.y, destino.y);

	// Verificar si las coordenadas de la proyección están dentro de los rangos
	return proyeccion.x >= minX && proyeccion.x <= maxX && proyeccion.y >= minY && proyeccion.y <= maxY;
}

float calcularDistancia(Punto punto1, Punto punto2) {
	float dx = punto2.x - punto1.x;
	float dy = punto2.y - punto1.y;
	return sqrt(dx * dx + dy * dy);
}

float calcularDiferenciaDistancias(Motor24BYJ48 motor, Punto origenCarte, Punto destinoCarte) {
	float distanciaOrigen = calcularDistancia(motor.getEsquina(), origenCarte);
	Serial.print("Distancia Origen: ");
	Serial.println(distanciaOrigen);

	float distanciaDestino = calcularDistancia(motor.getEsquina(), destinoCarte);
	Serial.print("Distancia Destino: ");
	Serial.println(distanciaDestino);

	Serial.print("Delta: ");
	Serial.println(distanciaOrigen - distanciaDestino);
	Serial.println();

	return  distanciaOrigen - distanciaDestino;
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

bool validarInputString(String input) {
	if (input.length() != 6) {
		return false;
	}

	char pieza = input[0];
	char origenColumna = input[1];
	char origenFila = input[2];
	char destinoColumna = input[4];
	char destinoFila = input[5];

	//if (!isalpha(pieza) || !islower(pieza)) {
	//	return false;
	//}

	if (origenColumna < 'a' || origenColumna > 'h' || destinoColumna < 'a' || destinoColumna > 'h') {
		return false;
	}

	if (origenFila < '1' || origenFila > '8' || destinoFila < '1' || destinoFila > '8') {
		return false;
	}

	return true;
}

void calcularDiferenciasDistancia() {
	Deltas[0] = calcularDiferenciaDistancias(motor1, origenCarte, destinoCarte);
	Deltas[1] = calcularDiferenciaDistancias(motor2, origenCarte, destinoCarte);
	Deltas[2] = calcularDiferenciaDistancias(motor3, origenCarte, destinoCarte);
	Deltas[3] = calcularDiferenciaDistancias(motor4, origenCarte, destinoCarte);
}

void establecerPasosMotores() {
	motor1.setPasos(Deltas[0]);
	motor2.setPasos(Deltas[1]);
	motor3.setPasos(Deltas[2]);
	motor4.setPasos(Deltas[3]);
}

void asignarIntervalos(Motor24BYJ48& motor1, Motor24BYJ48& motor2, Motor24BYJ48& motor3, Motor24BYJ48& motor4) {
	int maxPasos = max(max(abs(motor1.getPasos()), abs(motor2.getPasos())), max(abs(motor3.getPasos()), abs(motor4.getPasos())));

	motor1.setIntervalo(MIN_INTERVALO * maxPasos / abs(motor1.getPasos()));
	motor2.setIntervalo(MIN_INTERVALO * maxPasos / abs(motor2.getPasos()));
	motor3.setIntervalo(MIN_INTERVALO * maxPasos / abs(motor3.getPasos()));
	motor4.setIntervalo(MIN_INTERVALO * maxPasos / abs(motor4.getPasos()));

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

void mueveOrto() {
	calcularDiferenciasDistancia();
	establecerPasosMotores();
	asignarIntervalos(motor1, motor2, motor3, motor4);
	asignado = true;
}

void mueveDiagonal() {
	Motor24BYJ48* motorLateral1;
	Motor24BYJ48* motorLateral2;
	Motor24BYJ48* motorDiagonal3;
	Motor24BYJ48* motorDiagonal4;

	//determinar los motores laterales ...
	if (destinoCarte.x > origenCarte.x && destinoCarte.y > origenCarte.y) {
		// Movimiento hacia arriba a la derecha
		motorLateral1 = &motor2;
		motorLateral2 = &motor3;
		motorDiagonal3 = &motor4;
		motorDiagonal4 = &motor1;
	}
	else if (destinoCarte.x < origenCarte.x && destinoCarte.y > origenCarte.y) {
		// Movimiento hacia arriba a la izquierda
		motorLateral1 = &motor3;
		motorLateral2 = &motor4;
		motorDiagonal3 = &motor1;
		motorDiagonal4 = &motor2;
	}
	else if (destinoCarte.x < origenCarte.x && destinoCarte.y < origenCarte.y) {
		// Movimiento hacia abajo a la izquierda
		motorLateral1 = &motor4;
		motorLateral2 = &motor1;
		motorDiagonal3 = &motor2;
		motorDiagonal4 = &motor3;
	}
	else if (destinoCarte.x > origenCarte.x && destinoCarte.y < origenCarte.y) {
		// Movimiento hacia abajo a la derecha
		motorLateral1 = &motor1;
		motorLateral2 = &motor2;
		motorDiagonal3 = &motor3;
		motorDiagonal4 = &motor4;
	}

	// calcular proyecciones
	Proyecciones proyecciones = calcularProyecciones(origenCarte, destinoCarte, motorLateral1->getEsquina(), motorLateral2->getEsquina());

	// verificar si las proyecciones estan dentro del segmento
	bool proyeccionMotor1EnSegmento = estaDentroDelSegmento(origenCarte, destinoCarte, proyecciones.motor1);
	bool proyeccionMotor2EnSegmento = estaDentroDelSegmento(origenCarte, destinoCarte, proyecciones.motor2);

	if (!proyeccionMotor1EnSegmento && !proyeccionMotor2EnSegmento) {
		// Ningún motor lateral se proyecta sobre la trayectoria
		mueveOrto();
	}
	else if (proyeccionMotor1EnSegmento ^ proyeccionMotor2EnSegmento) {
		// Solo un motor lateral se proyecta sobre la trayectoria
		// Primera etapa: mover a la proyección
		destinoCarte = proyeccionMotor1EnSegmento ? proyecciones.motor1 : proyecciones.motor2;
		mueveOrto();

		// Esperar a que termine el movimiento
		moverMotoresHastaFinal(motor1, motor2, motor3, motor4);

		// Segunda etapa: mover al destino final
		origenCarte = destinoCarte;
		destinoCarte = casillaToCoordenadas(casillaDestino);
		mueveOrto();
	}
	else {
		// Ambos motores laterales se proyectan sobre la trayectoria
		// Primera etapa: mover a la primera proyección
		destinoCarte = proyecciones.motor1;
		mueveOrto();

		// Esperar a que termine el movimiento
		moverMotoresHastaFinal(motor1, motor2, motor3, motor4);

		// Segunda etapa: mover a la segunda proyección
		origenCarte = destinoCarte;
		destinoCarte = proyecciones.motor2;
		mueveOrto();

		// Esperar a que termine el movimiento
		moverMotoresHastaFinal(motor1, motor2, motor3, motor4);

		// Tercera etapa: mover al destino final
		origenCarte = destinoCarte;
		destinoCarte = casillaToCoordenadas(casillaDestino);
		mueveOrto();
	}
}

void moverMotoresSinc(Motor24BYJ48& motor1, unsigned long intervalo1, Motor24BYJ48& motor2, unsigned long intervalo2, Motor24BYJ48& motor3, unsigned long intervalo3, Motor24BYJ48& motor4, unsigned long intervalo4) {
	static unsigned long ultimoPasoMotor1 = 0;
	static unsigned long ultimoPasoMotor2 = 0;
	static unsigned long ultimoPasoMotor3 = 0;
	static unsigned long ultimoPasoMotor4 = 0;

	unsigned long tiempoActual = micros();

	if (tiempoActual - ultimoPasoMotor1 >= intervalo1) {
		motor1.darPaso();
		//Serial.print("Dando paso motor 1: ");
		//Serial.println(motor1.getPasos());

		ultimoPasoMotor1 = tiempoActual;
	}

	if (tiempoActual - ultimoPasoMotor2 >= intervalo2) {
		motor2.darPaso();
		//Serial.print("Dando paso motor 2: ");
		//Serial.println(motor2.getPasos());

		ultimoPasoMotor2 = tiempoActual;
	}

	if (tiempoActual - ultimoPasoMotor3 >= intervalo3) {
		motor3.darPaso();
		//Serial.println("Dando paso motor 3; ");
		//Serial.println(motor3.getPasos());

		ultimoPasoMotor3 = tiempoActual;
	}

	if (tiempoActual - ultimoPasoMotor4 >= intervalo4) {
		motor4.darPaso();
		//Serial.println("Dando paso motor 4");
		//Serial.println(motor4.getPasos());

		ultimoPasoMotor4 = tiempoActual;
	}
}

void moverMotoresHastaFinal(Motor24BYJ48& motor1, Motor24BYJ48& motor2, Motor24BYJ48& motor3, Motor24BYJ48& motor4) {
	while (asignado) {
		moverMotoresSinc(motor1, motor1.getIntervalo(), motor2, motor2.getIntervalo(), motor3, motor3.getIntervalo(), motor4, motor4.getIntervalo());

		if (motor1.getPasos() == 0 && motor2.getPasos() == 0 && motor3.getPasos() == 0 && motor4.getPasos() == 0) {
			asignado = false;
			motor1.stop();
			motor2.stop();
			motor3.stop();
			motor4.stop();
		}
	}
}

void setup() {
	Serial.begin(9600);
	inputString.reserve(200);
}

void loop(){
	if (Serial.available()) {
		inputString = Serial.readStringUntil('\n');
		stringComplete = true;
	}

	if (stringComplete) {
		if (inputString.length() == 2) {
			casillaOrigen = casillaAnterior;
			casillaDestino = notacionChessToOrdinalCasilla(inputString[0], inputString[1]);
			origenCarte = casillaToCoordenadas(casillaOrigen);
			destinoCarte = casillaToCoordenadas(casillaDestino);
			capture = false;
			pieza = 'p';
		}
		else {
			if (!validarInputString(inputString)) {
				Serial.println();
				Serial.println("Entrada invalida");
				Serial.println("Intente de nuevo");
				Serial.print(inputString);
				inputString = "";
				stringComplete = false;
				return;
			}
			pieza = inputString[0];
			casillaOrigen = notacionChessToOrdinalCasilla(inputString[1], inputString[2]);
			origenCarte = casillaToCoordenadas(casillaOrigen);
			casillaDestino = notacionChessToOrdinalCasilla(inputString[4], inputString[5]);
			destinoCarte = casillaToCoordenadas(casillaDestino);
			capture = (inputString[3] == 'x');
		}
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
		casillaAnterior = casillaDestino;

		if (relativoOrto(origenCarte, destinoCarte))
		{
			mueveOrto();
		}
		else
		{
			mueveDiagonal();
		}

		if (asignado)
		{
			//moverMotoresSinc(motor1, motor1.getIntervalo(), motor2, motor2.getIntervalo(), motor3, motor3.getIntervalo(), motor4, motor4.getIntervalo());
			moverMotoresHastaFinal(motor1, motor2, motor3, motor4);

			if (motor1.getPasos() == 0 && motor2.getPasos() == 0 && motor3.getPasos() == 0 && motor4.getPasos() == 0)
			{
				asignado = false;
				motor1.stop();
				motor2.stop();
				motor3.stop();
				motor4.stop();
			}
		}
	}
	//Punto origenCarte = { 0, 0 };
	//Punto destinoCarte = { 30, 40 };
	//Serial.println("Origen: ");	
	//Serial.println(origenCarte.x);
	//Serial.println(origenCarte.y);
	//Serial.println("Destino: ");
	//Serial.println(destinoCarte.x);
	//Serial.println(destinoCarte.y);
	//Serial.println(calcularDistancia(origenCarte, destinoCarte));
	//assert(calcularDistancia(origenCarte, destinoCarte) == 50);
	//while (true){}

}

