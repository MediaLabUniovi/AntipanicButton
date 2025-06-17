#include "Accelerometer.h"
#include <Wire.h>
 
Adafruit_MPU6050 mpu;

void Accelerometer() {

 
// Inicializar MPU6050
  if (!mpu.begin(0x68)) {
    Serial.println(" No se pudo encontrar un MPU6050.");
    while (1) delay(10);
  }
  Serial.println("✅ MPU6050 inicializado correctamente.");

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  mpu.setMotionInterrupt(true);
  mpu.setMotionDetectionThreshold(20);
  mpu.setMotionDetectionDuration(1.0);

}


