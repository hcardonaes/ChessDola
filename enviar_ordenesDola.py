import serial
import time

# Configurar la conexión serial
ser = serial.Serial('COM12', 9600)
ser.flush()
time.sleep(2)  # Espera a que se establezca la conexión

# Lista de comandos a enviar
comandos = ['rd4 d6\n', 'rd6 d2\n', 'rd2 f2\n', 'rf2 c6\n', 'rc6 d6\n', 'rd6 d4\n']
#comandos = ['rd6 d4\n']
for comando in comandos :
	print(f"Enviando: {comando.strip()}")
	ser.write(comando.encode('utf-8'))
	while True :
		if ser.in_waiting > 0 :
			respuesta = ser.readline().decode('utf-8').strip()
			print(f"Respuesta del Arduino ino: {respuesta}")
			if respuesta == "Instruccion procesada" :
				break
	time.sleep(10)  # Esperar un poco antes de enviar el siguiente comando
