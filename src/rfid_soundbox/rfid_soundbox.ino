/**
 * ----------------------------------------------------------------------------
 * This is the base program for the rfid sound-box project.
 * 
 */

#include "mfrc522_singleton.h"
#include "audioshield_singleton.h"
using namespace std;

/**
 * external variables.
 */
bool ISR_IsNewCardPresent;
string currentSong {};
string lastSong {};
string cardData {};

/**
 * Initialize.
 */
void setup() {
  Serial.begin(9600);               // Initialize serial communications with the PC
  delay(2000);
  Serial.println("LOADING>>>");

  audioshield_setup();
  mfrc_setup();

  ISR_IsNewCardPresent = false;
  currentSong = "IDLE";
}

/**
 * Main loop.
 */
void loop() {
  if (!ISR_IsNewCardPresent) {
    ISR_IsNewCardPresent = false;
    
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;
  
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    cardData = parsePICCMemory();

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();

    lastSong = currentSong;
    currentSong = cardData;

    // Check if valid
    if ( cardData.size() == 0)
      return;
  }

  string songTitle = currentSong;
  Serial.println(&songTitle[0]);
  
  // Play song
  playFile(&songTitle[0]);
  delay(500);
}
