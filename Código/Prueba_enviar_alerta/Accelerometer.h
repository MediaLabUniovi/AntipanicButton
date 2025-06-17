// Accelerometer.h
#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <Wire.h>
#include <Adafruit_MPU6050.h>

// Declaración externa de la variable mpu
extern Adafruit_MPU6050 mpu;
extern bool ACCELEROMETER_DETECTED;

void Accelerometer();  // Prototipo para la función de alerta

#endif
