import serial

# Configura tu conexión serial aquí
ser = serial.Serial('COM12', 9600, timeout=1)  # Asegúrate de usar el puerto correcto

def enviar_comando_y_esperar_respuesta(comando):
    ser.write(comando.encode('utf-8'))  # Enviar el comando al Arduino
    while True:
        if ser.in_waiting > 0:
            respuesta = ser.readline().decode('utf-8').strip()
            if respuesta == "Instruccion procesada":
                print("Arduino: ", respuesta)
                break  # Salir del bucle cuando recibas la confirmación

# Ejemplo de uso
enviar_comando_y_esperar_respuesta("td4 d6")
enviar_comando_y_esperar_respuesta("td6 f6")
enviar_comando_y_esperar_respuesta("tf6 f4")
enviar_comando_y_esperar_respuesta("tf4 d4")



