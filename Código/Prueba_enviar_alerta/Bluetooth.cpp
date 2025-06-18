#include "BluetoothSerial.h"

BluetoothSerial SerialBT;


// Esta función se llamará si falla la conexión LoRa
void enviarAlertaPorBluetooth() {

  SerialBT.begin("ESP32_PANIC");  // Nombre del dispositivo Bluetooth
  Serial.println("Bluetooth listo.");

  if (SerialBT.hasClient()) {
    Serial.println("Dispositivo Bluetooth conectado. Enviando alerta...");
    SerialBT.println("PANIC_ALERT");
  } else {
    Serial.println("No hay dispositivo Bluetooth conectado.");
  }

}
