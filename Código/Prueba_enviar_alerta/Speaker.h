#ifndef Speaker_H
#define Speaker_H

#include "DFRobotDFPlayerMini.h"
#include "Arduino.h"

#define DFPLAYER_NEXT_PIN 15  // Pin de la LilyGO conectado a IO2 del DFPlayer Mini

  

extern DFRobotDFPlayerMini myDFPlayer;

// Prototipos de funciones
void setupDFPlayer();
void resetearDFPlayer();
void SiguienteAudio();

#endif
