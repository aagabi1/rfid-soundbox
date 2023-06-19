#ifndef GLOBAL_INCLUDE_H
#define GLOBAL_INCLUDE_H

#include <Arduino.h>
#include <iostream>
#include <vector>
#include <SPI.h>

#include <MFRC522.h>

#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

#include "Adafruit_TPA2016.h"

using namespace std;

extern bool ISR_IsNewCardPresent;
extern string currentSong;
extern string lastSong;
#endif
