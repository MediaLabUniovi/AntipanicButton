#include "gps.h"

TinyGPSPlus gps;
HardwareSerial mySerialGPS(1);  // Puerto Serial 1 para GPS (RX: 14, TX: -1)

// Funci�n para inicializar el GPS
void setupGPS() {

  
  mySerialGPS.begin(9600, SERIAL_8N1, 14, -1);  // Configura el puerto serial para el GPS
  delay(2000);
  Serial.println("GPS listo");
}

// Función para obtener la latitud y longitud
void obtenerGPS(float &lat, float &lon) {
  lat = lon = 0.0; // Inicializa con valores inválidos
  unsigned long startTime = millis();

  while (millis() - startTime < 3000) { // Espera hasta 3 segundos
    while (mySerialGPS.available()) {
      char c = mySerialGPS.read();
      gps.encode(c);
      if (gps.location.isUpdated()) { // Verifica si hay una nueva lectura válida
        lat = gps.location.lat();
        lon = gps.location.lng();
        Serial.print("Latitud: "); Serial.println(lat, 6);
        Serial.print("Longitud: "); Serial.println(lon, 6);
        return; // Salir si se obtuvo una lectura válida
      }
    }
  }

  Serial.println("No se ha recibido un dato GPS válido.");
}
   



