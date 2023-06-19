#ifndef AUDIO_SHIELD_H
#define AUDIO_SHIELD_H

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

#include "globalinclude.h"

using namespace std;

void audioshield_setup();
void playFile(const char *filename);
void toggle();

#endif
