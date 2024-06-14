import serial
import time

# Abre el puerto serie (reemplaza '/dev/ttyACM0' con el puerto correcto)
arduino = serial.Serial('COM12', 9600)
time.sleep(2)  # Espera a que se establezca la conexión

# Abre el archivo de texto
with open('movimientos.txt', 'r') as file:
    for line in file:
        line = line.strip()  # Elimina los espacios en blanco y las nuevas líneas
        arduino.write(line.encode())  # Envía la línea al Arduino

        # Espera a que el Arduino envíe una respuesta
        while arduino.inWaiting() == 0:
            pass

        # Lee la respuesta del Arduino
        respuesta = arduino.readline().decode().strip()
        print("Respuesta del Arduino: ", respuesta)

        time.sleep(1)  # Espera a que el Arduino procese los datos

arduino.close()  # Cierra la conexión
