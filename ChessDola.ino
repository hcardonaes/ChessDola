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

Motor24BYJ48* motorLateral1;
Motor24BYJ48* motorLateral2;

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
	//ñ Serial.print("Pendiente: ");
	//ñ Serial.println(m);

	// Calcular el término independiente de la línea
	float b = origen.y - m * origen.x;
	//ñ Serial.print("Término independiente: ");
	//ñ Serial.println(b);


	// Calcular las proyecciones de los motores sobre la línea
	Proyecciones proyecciones;
	proyecciones.motor1.x = (m * motor1.y + motor1.x - m * b) / (m * m + 1);
	proyecciones.motor1.y = (m * m * motor1.y + m * motor1.x + b) / (m * m + 1);
	proyecciones.motor2.x = (m * motor2.y + motor2.x - m * b) / (m * m + 1);
	proyecciones.motor2.y = (m * m * motor2.y + m * motor2.x + b) / (m * m + 1);
	//ñ Serial.print("Proyeccion motor 1: ");
	//ñ Serial.println(proyecciones.motor1.x);
	//ñ Serial.println(proyecciones.motor1.y);
	//ñ Serial.print("Proyeccion motor 2: ");
	//ñ Serial.println(proyecciones.motor2.x);
	//ñ Serial.println(proyecciones.motor2.y);

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
	//ñ Serial.print("Distancia Origen: ");//ñ Serial.print(motor.numMotor);	//ñ Serial.print("   ");
	//ñ Serial.println(distanciaOrigen);

	float distanciaDestino = calcularDistancia(motor.getEsquina(), destinoCarte);
	//ñ Serial.print("Distancia Destino: ");
	//ñ Serial.println(distanciaDestino);

	//ñ Serial.print("Delta: ");
	//ñ Serial.println(distanciaOrigen - distanciaDestino);
	//ñ Serial.println();

	return  distanciaOrigen - distanciaDestino;
}

bool relativoOrto(Punto origenCarte, Punto destinoCarte)
{
	float dx = destinoCarte.x - origenCarte.x;
	float dy = destinoCarte.y - origenCarte.y;
	if (dx == 0 || dy == 0) {
		return true;
		//ñ Serial.println("Es ortogonal");
	}
	if (abs(dx) == abs(dy)) {
		//ñ Serial.println("Es diagonal");
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
	long maxPasos = max(max(abs(motor1.getPasos()), abs(motor2.getPasos())), max(abs(motor3.getPasos()), abs(motor4.getPasos())));

	long steps1 = abs(motor1.getPasos());
	long steps2 = abs(motor2.getPasos());
	long steps3 = abs(motor3.getPasos());
	long steps4 = abs(motor4.getPasos());

	long calculoIntervalo1 = abs(MIN_INTERVALO * maxPasos / steps1);
	long calculoIntervalo2 = abs(MIN_INTERVALO * maxPasos / steps2);
	long calculoIntervalo3 = abs(MIN_INTERVALO * maxPasos / steps3);
	long calculoIntervalo4 = abs(MIN_INTERVALO * maxPasos / steps4);

	//ñ Serial.print("Calculo intervalo 1: ");
	//ñ Serial.println(calculoIntervalo1);
	motor1.setIntervalo(calculoIntervalo1);
	//ñ Serial.print("Calculo intervalo 2: ");
	//ñ Serial.println(calculoIntervalo2);
	motor2.setIntervalo(calculoIntervalo2);
	//ñ Serial.print("Calculo intervalo 3: ");
	//ñ Serial.println(calculoIntervalo3);
	motor3.setIntervalo(calculoIntervalo3);
	//ñ Serial.print("Calculo intervalo 4: ");
	//ñ Serial.println(calculoIntervalo4);
	motor4.setIntervalo(calculoIntervalo4);

	//ñ Serial.print("Intervalo motor 1: ");
	//ñ Serial.println(motor1.getIntervalo());
	//ñ Serial.print("Intervalo motor 2: ");
	//ñ Serial.println(motor2.getIntervalo());
	//ñ Serial.print("Intervalo motor 3: ");
	//ñ Serial.println(motor3.getIntervalo());
	//ñ Serial.print("Intervalo motor 4: ");
	//ñ Serial.println(motor4.getIntervalo());
}

void preparaMovOrto() {
	//ñ Serial.println("Movimiento ortogonal");
	calcularDiferenciasDistancia();
	establecerPasosMotores();
	asignarIntervalos(motor1, motor2, motor3, motor4);
	asignado = true;
}

void motoresLaterales() {


	//determinar los motores laterales ...
	if (destinoCarte.x > origenCarte.x && destinoCarte.y > origenCarte.y) {
		// Movimiento hacia arriba a la derecha
		motorLateral1 = &motor2;
		motorLateral2 = &motor4;
	}
	else if (destinoCarte.x < origenCarte.x && destinoCarte.y > origenCarte.y) {
		// Movimiento hacia arriba a la izquierda
		motorLateral1 = &motor1;
		motorLateral2 = &motor3;
	}
	else if (destinoCarte.x < origenCarte.x && destinoCarte.y < origenCarte.y) {
		// Movimiento hacia abajo a la izquierda
		motorLateral1 = &motor2;
		motorLateral2 = &motor4;
	}
	else if (destinoCarte.x > origenCarte.x && destinoCarte.y < origenCarte.y) {
		// Movimiento hacia abajo a la derecha
		motorLateral1 = &motor1;
		motorLateral2 = &motor3;
	}
}

void preparaMovDiagonal() {
	// determinar los motores laterales
	motoresLaterales();

	//ñ Serial.println("Movimiento diagonal");
	//ñ Serial.print("Motor lateral 1: ");
	//ñ Serial.println(motorLateral1->numMotor);
	//ñ Serial.print("Motor lateral 2: ");
	//ñ Serial.println(motorLateral2->numMotor);

	// calcular proyecciones
	Proyecciones proyecciones = calcularProyecciones(origenCarte, destinoCarte, motorLateral1->getEsquina(), motorLateral2->getEsquina());

	// verificar si las proyecciones estan dentro del segmento
	bool proyeccionMotor1EnSegmento = estaDentroDelSegmento(origenCarte, destinoCarte, proyecciones.motor1);
	bool proyeccionMotor2EnSegmento = estaDentroDelSegmento(origenCarte, destinoCarte, proyecciones.motor2);

	if (!proyeccionMotor1EnSegmento && !proyeccionMotor2EnSegmento) {
		// Ningún motor lateral se proyecta sobre la trayectoria
		//ñ Serial.println("Ningún motor lateral se proyecta sobre la trayectoria");
		preparaMovOrto();
	}
	else if (proyeccionMotor1EnSegmento ^ proyeccionMotor2EnSegmento) {
		// Solo un motor lateral se proyecta sobre la trayectoria
		//ñ Serial.println("Solo un motor lateral se proyecta sobre la trayectoria");
		// Primera etapa: mover a la proyección
		destinoCarte = proyeccionMotor1EnSegmento ? proyecciones.motor1 : proyecciones.motor2;
		//ñ Serial.print("Proyecta solo el ");
		//ñ Serial.println(proyeccionMotor1EnSegmento ? "motor 1" : "motor 2");
		//ñ Serial.print("Origen x: ");
		//ñ Serial.println(origenCarte.x);
		//ñ Serial.print("Origen y: ");
		//ñ Serial.println(origenCarte.y);
		//ñ Serial.print("Destino x: ");
		//ñ Serial.println(destinoCarte.x);
		//ñ Serial.print("Destino y: ");
		//ñ Serial.println(destinoCarte.y);

		preparaMovOrto();

		// Esperar a que termine el movimiento
		moverMotoresHastaFinal(motor1, motor2, motor3, motor4);

		// Segunda etapa: mover al destino final
		//ñ Serial.println("Para la segunda etapa: ");
		origenCarte = destinoCarte;
		destinoCarte = casillaToCoordenadas(casillaDestino);
		//ñ Serial.print("Origen x: ");
		//ñ Serial.println(origenCarte.x);
		//ñ Serial.print("Origen y: ");
		//ñ Serial.println(origenCarte.y);
		//ñ Serial.print("Destino x: ");
		//ñ Serial.println(destinoCarte.x);
		//ñ Serial.print("Destino y: ");
		//ñ Serial.println(destinoCarte.y);

		preparaMovOrto();
	}
	else {
		// Ambos motores laterales se proyectan sobre la trayectoria
		// Primera etapa: mover a la primera proyección
		//ñ Serial.println("Ambos motores laterales se proyectan sobre la trayectoria");
		float distanciaProyeccion1 = calcularDistancia(origenCarte, proyecciones.motor1);
		float distanciaProyeccion2 = calcularDistancia(origenCarte, proyecciones.motor2);

		if (distanciaProyeccion1 < distanciaProyeccion2) {
			destinoCarte = proyecciones.motor1;
			// Guardar la segunda proyección para la siguiente etapa
			proyecciones.motor1 = proyecciones.motor2;
		}

		else {
			destinoCarte = proyecciones.motor2;
			// Guardar la segunda proyección para la siguiente etapa
			proyecciones.motor2 = proyecciones.motor1;
		}
		//ñ Serial.print("Distancia proyeccion 1: ");
		//ñ Serial.println(distanciaProyeccion1);
		//ñ Serial.print("Distancia proyeccion 2: ");
		//ñ Serial.println(distanciaProyeccion2);
		//ñ Serial.println();
		//ñ Serial.println("Primera etapa: ");
		//ñ Serial.print("Origen x: ");
		//ñ Serial.println(origenCarte.x);
		//ñ Serial.print("Origen y: ");
		//ñ Serial.println(origenCarte.y);
		//ñ Serial.print("Destino x: ");
		//ñ Serial.println(destinoCarte.x);
		//ñ Serial.print("Destino y: ");
		//ñ Serial.println(destinoCarte.y);


		preparaMovOrto();

		// Esperar a que termine el movimiento
		moverMotoresHastaFinal(motor1, motor2, motor3, motor4);

		// Segunda etapa: mover a la segunda proyección
		//ñ Serial.println("Para la segunda etapa: ");	
		origenCarte = destinoCarte;
		destinoCarte = proyecciones.motor2;
		//ñ Serial.print("Origen x: ");
		//ñ Serial.println(origenCarte.x);
		//ñ Serial.print("Origen y: ");
		//ñ Serial.println(origenCarte.y);
		//ñ Serial.print("Destino x: ");
		//ñ Serial.println(destinoCarte.x);
		//ñ Serial.print("Destino y: ");
		//ñ Serial.println(destinoCarte.y);

		preparaMovOrto();

		// Esperar a que termine el movimiento
		moverMotoresHastaFinal(motor1, motor2, motor3, motor4);

		// Tercera etapa: mover al destino final
		//ñ Serial.println("Para la tercera etapa: ");
		origenCarte = destinoCarte;
		destinoCarte = casillaToCoordenadas(casillaDestino);
		//ñ Serial.print("Origen x: ");
		//ñ Serial.println(origenCarte.x);
		//ñ Serial.print("Origen y: ");
		//ñ Serial.println(origenCarte.y);
		//ñ Serial.print("Destino x: ");
		//ñ Serial.println(destinoCarte.x);
		//ñ Serial.print("Destino y: ");
		//ñ Serial.println(destinoCarte.y);

		preparaMovOrto();
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
				//ñ Serial.println();
				//ñ Serial.println("Entrada invalida");
				//ñ Serial.println("Intente de nuevo");
				//ñ Serial.print(inputString);
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
			//ñ Serial.print("input: ");
			//ñ Serial.println(inputString);
			//ñ Serial.print("Origen x: ");
			//ñ Serial.println(casillaOrigen.x);
			//ñ Serial.print("Origen y: ");
			//ñ Serial.println(casillaOrigen.y);
			//ñ Serial.print("Origen mm x: ");
			//ñ Serial.println(origenCarte.x);
			//ñ Serial.print("Origen mm y: ");
			//ñ Serial.println(origenCarte.y);
			//ñ Serial.print("Destino x: ");
			//ñ Serial.println(casillaDestino.x);
			//ñ Serial.print("Destino y: ");
			//ñ Serial.println(casillaDestino.y);
			//ñ Serial.print("Destino x mm: ");
			//ñ Serial.println(destinoCarte.x);
			//ñ Serial.print("Destino y mm: ");
			//ñ Serial.println(destinoCarte.y);
			//ñ Serial.print("Capture: ");
			//ñ Serial.println(capture);
			//ñ Serial.print("pieza: ");
			//ñ Serial.println(pieza);
		}
		inputString = "";
		stringComplete = false;
		casillaAnterior = casillaDestino;

		if (relativoOrto(origenCarte, destinoCarte))
		{
			preparaMovOrto();
		}
		else
		{
			preparaMovDiagonal();
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

