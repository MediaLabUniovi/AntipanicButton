#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h>
#include <HardwareSerial.h>

extern TinyGPSPlus gps;
extern HardwareSerial mySerial;

// Inicializa el GPS
void setupGPS();

// Lee y obtiene los datos de latitud y longitud
void obtenerGPS(float &lat, float &lon);

// Prepara el payload con los datos GPS para enviar a TTN
void enviarDatosGPS();

#endif

