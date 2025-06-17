#ifndef ONEVENTHANDLER_H
#define ONEVENTHANDLER_H

#include <lmic.h>
#include <Arduino.h>
#include <Speaker.h>

// Pines de LEDs
#define LED_PIN_G 12  // LED verde
#define LED_PIN_R 2   // LED rojo

// Estados del sistema
enum State {
    IDLE,
    WAITING_FOR_OK,
    WAITING_FOR_HELP
};

extern volatile bool joined;
extern volatile State currentState;
extern volatile bool txDone;
extern volatile uint8_t lastDownlink;

void onEvent(ev_t ev);

#endif
