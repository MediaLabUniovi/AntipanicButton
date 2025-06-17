
#include "Speaker.h"

// Creamos un objeto Serial1 con pines asignados manualmente
HardwareSerial SerialDFP(2);  // UART1

DFRobotDFPlayerMini myDFPlayer;

void setupDFPlayer() {
  Serial.begin(115200);  // Para debug

  // Iniciar UART1 en pines 16 (RX) y 17 (TX) a 9600 baudios
  SerialDFP.begin(9600, SERIAL_8N1, -1, 25);
  delay(2000);

  Serial.println("Iniciando DFPlayer...");

  if (!myDFPlayer.begin(SerialDFP)) {
    Serial.println(" No se pudo comunicar con DFPlayer.");
    Serial.println("Verifica cableado, SD y alimentación.");
    //while (true) delay(1000);
    return;
  }

  Serial.println(" DFPlayer iniciado correctamente.");

  myDFPlayer.volume(3);  // Volumen (0-30)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);  // Fuente: tarjeta SD

  Serial.println("Inicializado del todo");
}


void resetearDFPlayer() {
  
  Serial.println("sigo1");
  delay(500);
  SerialDFP.end();
  delay(500);
  Serial.println("sigo2");


  SerialDFP.begin(9600, SERIAL_8N1, -1, 25);
  delay(2000);
  myDFPlayer.volume(30);  // Volumen (0-30)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);  // Fuente: tarjeta SD
  Serial.println("sigo3");


}

void SiguienteAudio() {

  digitalWrite(DFPLAYER_NEXT_PIN, LOW);   // Activar pulso bajo
  delay(100);                             // Mantenerlo 100 ms
  digitalWrite(DFPLAYER_NEXT_PIN, HIGH);  // Volver a estado inactivo

}




