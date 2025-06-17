#include "onEventHandler.h"

#include "DFRobotDFPlayerMini.h"
#include "Arduino.h"
#include "Speaker.h"


volatile bool joined = false;
volatile State currentState = IDLE;
volatile bool txDone = false;
volatile uint8_t lastDownlink = 0x00;

void onEvent(ev_t ev) {
    switch (ev) {
        case EV_JOINING:
            Serial.println(F("Unión en progreso..."));
            break;
        case EV_JOINED:
            Serial.println(F("¡Dispositivo unido a TTN!"));
            joined = true;
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("Fallo al intentar unirse a TTN"));
            break;
        case EV_TXCOMPLETE:
            Serial.println("📡 Uplink enviado");
            txDone = true;

            if (LMIC.dataLen > 0) {
                lastDownlink = LMIC.frame[LMIC.dataBeg];
                Serial.print("🔻 Downlink recibido: ");
                Serial.println(lastDownlink, HEX);
                
                if (lastDownlink != 0x00 && currentState == WAITING_FOR_OK) {
                    myDFPlayer.play(2); //Audio OK
                    Serial.println("✅ OK recibido. LED verde encendido.");
                    digitalWrite(LED_PIN_G, HIGH);
                    delay(10000);
                    digitalWrite(LED_PIN_G, LOW);


                    currentState = WAITING_FOR_HELP;
                    lastDownlink= 0x00;
                    // Iniciar UART1 en pines 16 (RX) y 17 (TX) a 9600 baudios
                    
                    
                } else if (lastDownlink != 0x00 && currentState == WAITING_FOR_HELP) {
                                        
                    //resetearDFPlayer();
                    SiguienteAudio();
                    //delay(100);
    
                    Serial.println("🆘 Ayuda en camino. LED verde parpadeando.");                    
                    for (int i = 0; i < 10; i++) {
                        digitalWrite(LED_PIN_G, HIGH);
                        delay(500);
                        digitalWrite(LED_PIN_G, LOW);
                        delay(500);
                    }
                    //txdone= true;
                    currentState = IDLE;
                }
            }
            break;

        default:
            break;
    }
}
