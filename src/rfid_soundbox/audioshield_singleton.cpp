/**
 * ----------------------------------------------------------------------------
 * This is the single instance for the audio shield (sgt5000_1).
 * All that will be included here is code that handles playback functionality
 * including but not limited to:
 * 1. playing a song based on filename
 * 2. handling interrupts for pauing, switching songs
 * 
 */

#include "audioshield_singleton.h"
#include "mfrc522_singleton.h"
#define BOUNCE_DURATION   50
#define DOUBLE_CLICK_TIME    700

/**
 * User input variables.
 */
const byte playPausePin = 32;
unsigned int lastHoldTime = 0;
// unsigned int clickCount = 0;
// unsigned long lastClickTime = 0;
Bounce playPauseButton = Bounce(playPausePin, 25);

/**
 * Audio variables.
 */
AudioPlaySdWav           playSdWav;     //xy=260,765
AudioAmplifier           amp1;           //xy=480,699
AudioAmplifier           amp2;           //xy=500,790
AudioOutputI2S           audioOutput;           //xy=874,761
AudioConnection          patchCord1(playSdWav, 0, amp1, 0);
AudioConnection          patchCord2(playSdWav, 1, amp2, 0);
AudioConnection          patchCord3(amp1, 0, audioOutput, 0);
AudioConnection          patchCord4(amp2, 0, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=404,514

void audioshield_setup() {
  // Setup amp
  Adafruit_TPA2016 audioamp = Adafruit_TPA2016();
  audioamp.begin();
  audioamp.setAGCCompression(TPA2016_AGC_OFF);
  audioamp.setReleaseControl(0);
  audioamp.setLimitLevelOn();
  audioamp.setLimitLevel(1.5);
  Serial.println("Amp setup complete");

  // Handle user input
  pinMode(playPausePin, INPUT);

  // Setup audioshield
  AudioMemory(8);                   // Audio connections require memory to work

  sgtl5000_1.enable();              // Turn on audio shield chip
  sgtl5000_1.volume(0.5);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playSdWav.play(filename);

  // A brief delay for the library read WAV info
  delay(25);

  // Simply wait for the file to finish playing.
  while (!playSdWav.isStopped()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    float vol = analogRead(15);
    vol = vol / 1024;
    amp1.gain(vol);
    amp2.gain(vol);
    sgtl5000_1.volume(vol);

  
    if (playPauseButton.update()) {
      if (playPauseButton.fallingEdge()) {
          playSdWav.togglePlayPause();
          lastHoldTime = millis();
          Serial.println("Pause/play");
      }
      if (playPauseButton.risingEdge()) {
        lastHoldTime = 0;
      }
    } else {
      if (millis() - lastHoldTime > 2000 && lastHoldTime > 0) {
        Serial.println("Stopping playback...");
        lastHoldTime = 0;
        playSdWav.stop();
        break;
      }
    }

    // Check new card
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      Serial.println("New card detected...");
      string cardData = parsePICCMemory();

      // Halt PICC
      mfrc522.PICC_HaltA();
      // Stop encryption on PCD
      mfrc522.PCD_StopCrypto1();

      // Handle new card data
      if (cardData.size() > 0) {
        Serial.println(&cardData[0]);
        Serial.println(&currentSong[0]);
        Serial.println(cardData.compare(currentSong));
        if (cardData.compare(currentSong)) {
          lastSong = currentSong;
          currentSong = cardData;
          ISR_IsNewCardPresent = true;
          break;
        }
      }
    }
  }
  Serial.println("Finished playback");
}
