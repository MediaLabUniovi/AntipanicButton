//Eventos LoRa
#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "onEventHandler.h"

//GPS
#include <TinyGPS.h>
#include <HardwareSerial.h>
#include "gps.h"  

//Acelerómetro
#include <Wire.h>
#include <MPU6050.h>
#include <math.h>
#include "Accelerometer.h"

//Altavoz
#include "Speaker.h"

//Propio ESP32
#include "esp_sleep.h"

// Claves de TTN
static const u1_t PROGMEM APPEUI[8] = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
static const u1_t PROGMEM DEVEUI[8] = { 0x24, 0x16, 0x07, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
static const u1_t PROGMEM APPKEY[16] = { 0x6C, 0x45, 0x97, 0x86, 0xE4, 0x3C, 0x54, 0x62, 0x95, 0x13, 0x63, 0x23, 0x03, 0x19, 0x15, 0x18 };

const lmic_pinmap lmic_pins = {
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 23,
    .dio = {26, 33, 32},
};

// Botón de pánico
#define BOTON_PIN 4
// Pin para la interrupción de la caída
#define MPU_INT_PIN 13  

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // tiempo que transcurre entre uplinks -->10 segundos
bool buttonPressed = false;
bool BUTTON_DETECTED = false;
bool ACCELEROMETER_DETECTED = false;


void os_getArtEui(u1_t* buf) { memcpy(buf, APPEUI, 8); }
void os_getDevEui(u1_t* buf) { memcpy(buf, DEVEUI, 8); }
void os_getDevKey(u1_t* buf) { memcpy(buf, APPKEY, 16); }

void setup() {
    Serial.begin(115200);
    delay(100);  // Pequeña pausa para que el serial arranque bien

    

    pinMode(BOTON_PIN, INPUT_PULLUP);
    pinMode(MPU_INT_PIN, INPUT);
    pinMode(DFPLAYER_NEXT_PIN, OUTPUT);
    pinMode(LED_PIN_R, OUTPUT);
    pinMode(LED_PIN_G, OUTPUT);
    digitalWrite(LED_PIN_R, LOW);
    digitalWrite(LED_PIN_G, LOW);
    digitalWrite(DFPLAYER_NEXT_PIN, HIGH);  // Estado inactivo


    /*esp_sleep_enable_ext0_wakeup((gpio_num_t)BOTON_PIN, 0);  // Despertar cuando el pin se pone en nivel bajo (0)

    //if (digitalRead(BOTON_PIN) == LOW ){
        BUTTON_DETECTED = true;
        Serial.println("BOTÓN DETECTADO");
    //}
*/
    
// Detectar causa del wake-up
  esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();

  if (reason == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("¡Caída detectada. Enviando alerta!");
    ACCELEROMETER_DETECTED = true;

  } else if (reason == ESP_SLEEP_WAKEUP_EXT1) {
    // Averiguar qué pin provocó el wake-up EXT1
      Serial.println("¡Botón presionado. Enviando alerta!");
      BUTTON_DETECTED = true;

    } 
   else {
    Serial.println("🔄 Encendido o reinicio normal.");
    BUTTON_DETECTED = true;
  }



    // 2️⃣ Configuramos el acelerómetro para futuras caídas
    Accelerometer();  // Inicializa MPU6050
    
    // 3️⃣ Configuramos el GPS
    setupGPS();

    //Configuramos el Altavoz
    setupDFPlayer();

    // 4️⃣ Configuramos LoRa
    SPI.begin(5, 19, 27, 18);
    os_init();
    LMIC_reset();
    LMIC_startJoining();
    LMIC.dn2Dr = DR_SF7;
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
    LMIC_setLinkCheckMode(0);
    LMIC_setAdrMode(0);
    
    // Configurar wakeup:
        // ext0: pin INT MPU6050 (HIGH)
        esp_sleep_enable_ext0_wakeup((gpio_num_t)MPU_INT_PIN, 1);

        // ext1: botón (nivel LOW)
        esp_sleep_enable_ext1_wakeup(1ULL << BOTON_PIN, ESP_EXT1_WAKEUP_ALL_LOW);


}

void loop() {
    os_runloop_once();

    
    if ((/*digitalRead(BOTON_PIN) == LOW ||*/ BUTTON_DETECTED || ACCELEROMETER_DETECTED) && currentState == IDLE && joined == true) {
        delay(500); // Pequeño rebote

        if (BUTTON_DETECTED){
        BUTTON_DETECTED = false;
        }
        if (ACCELEROMETER_DETECTED){
        ACCELEROMETER_DETECTED = false;
        }

        
        float lat, lon;
        obtenerGPS(lat,lon);

        // Preparar el payload combinado
        uint8_t payload[9];  // 1 byte para el botón + 4 bytes para latitud + 4 bytes para longitud
        
        payload[0] = 0x01;  // Payload del botón de pánico
        // Copiar latitud y longitud al payload (4 bytes cada uno)
        memcpy(payload + 1, &lat, 4);  // Latitud en los bytes 1 a 4
        memcpy(payload + 5, &lon, 4);  // Longitud en los bytes 5 a 8

        

        LMIC_setTxData2(1, payload, sizeof(payload), 0);

        myDFPlayer.play(1);//Audio Alerta enviada

        digitalWrite(LED_PIN_R, HIGH);  // LED rojo ON
        delay(10000);
        digitalWrite(LED_PIN_R, LOW);  // LED rojo OFF
        currentState = WAITING_FOR_OK;
        txDone = false;
        lastDownlink = 0;
        
        
        
    }



    // Enviar periódicamente si estamos esperando respuestas
    if ((currentState == WAITING_FOR_OK || currentState == WAITING_FOR_HELP) && millis() - lastSendTime > sendInterval) {
        if (!(LMIC.opmode & OP_TXRXPEND)) {
            uint8_t payload2 = (currentState == WAITING_FOR_OK) ? 0x03 : 0x05;
            LMIC_setTxData2(1, &payload2, 1, 0);
            Serial.println("📤 Enviando uplink para ver si hay downlink...");
            txDone = false;
            lastSendTime = millis();
        } else {
            Serial.println("⏳ Esperando a que termine el envío anterior...");
            delay(2000);
        }
    }

    // Si hemos terminado el proceso, entrar en deep sleep
    if (currentState == IDLE && txDone) {
        
        Serial.println("🛌 Proceso completado. Entrando en deep sleep...");
        delay(1000);  // Espera a que el serial imprima
        esp_deep_sleep_start();
    }
}

