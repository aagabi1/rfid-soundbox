/**
 * ----------------------------------------------------------------------------
 * This is the single instance for the mfrc522.
 * All that will be included here is code that handles rfid functionality
 * including but not limited to:
 * 1. reading data from and writing data to the picc.
 * 2. converting data back and forth from text to byte arrays.
 * 
 */

#include "mfrc522_singleton.h"
#define DATA_BLOCK_SIZE   2

/**
 * RFID PICC variables.
 */
MFRC522 mfrc522(SS_PIN, RST_PIN);     // Create MFRC522 instance
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

void mfrc_setup() {
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, HIGH);
  delay(1000);
  digitalWrite(RST_PIN, LOW);
  digitalWrite(RST_PIN, HIGH);
  SPI.begin();                      // Init SPI bus
  mfrc522.PCD_Init();               // Init MFRC522 card

  // Prepare the key (used both as key A and as key B)
  for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
  }

  Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
  Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));
}

/**
 * Handle all mfrc interactions.
 */
string parsePICCMemory() {
  // Variables for handling read and write menmory operations
  int memMarker       = 0;
  byte sector         = 1;
  byte blockAddr      = 4;
  byte trailerBlock   = 7;
  const char* targetId   = "EPIDEMIC/MOVEFLOW.WAV"; // filename must be 8.3 format
  vector<vector<byte>> dataBlock = string_to_bytes (targetId);
  
  string cardData {};

  for (size_t i = 0; i < DATA_BLOCK_SIZE; i++) { // chnage to 'i < dataBlock.size()' when writing
    // Authenticate using key A
    authenticate_picc(trailerBlock);

    // Write data to the block
    // write_to_picc(blockAddr, &dataBlock[i][0]);

    // Read data from the block (verification)
    vector<byte> my_buffer = read_from_picc(blockAddr);
    if (my_buffer == vector<byte> {(char) 9}) {
      Serial.println("Read failed");
      cardData = "";
      break;
    }
    cardData += byte_to_string(my_buffer);
    
    /** 
     * Update memory locations 
     * Caution:This is hardcoded to work with the MiFARE Classic 1k.
     * The memory location and offsets change depending on the PICC.
     */
    if (blockAddr > 62) {Serial.println("Invalid Operation: Exceeded memory blocks"); break;}
    
    if (memMarker == 2){
      sector += 1;
      memMarker = 0;
      blockAddr += 2;
      trailerBlock += 4;
    }   
    else {
      blockAddr += 1;
      memMarker += 1;
    }      
  }
  return cardData;
}

/**
 * Helper routine for authenticating, reading and writing.
 */
void authenticate_picc (uint8_t trailerBlock) {
  mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
}

void write_to_picc (uint8_t blockAddr, uint8_t* data) {
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  dump_byte_array(data, 16); Serial.println();
  mfrc522.MIFARE_Write(blockAddr, data, 16);
}

vector<byte> read_from_picc (uint8_t blockAddr) {
  byte buffer[18];
  byte size = sizeof(buffer);
  status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);

  Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
  dump_byte_array(buffer, 16); Serial.println("\n");

  if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Read() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return vector<byte> {(char) 9};
  }
  
  // Return payload
  int vecSize = sizeof(buffer) / sizeof(buffer[0]);
  vector<byte> data (buffer, buffer + vecSize);
  data.resize(16, 94);
  return data;
}

/**
 * Helper routine to convert string to multiple 16 byte vectors (and back to string).
 */
vector<vector<byte>> string_to_bytes (const char* str) {
  vector<vector<byte>> dataBlock;
  vector<byte> buffer;
  for (int i = 1; i < strlen(str) + 1; i++) {
    buffer.push_back((byte)str[i-1]);

    if (i % 16 == 0 && i != 0) {
      dataBlock.push_back(buffer);
      buffer.clear();
    }
  }

  // Add remaining buffered items
  if (!buffer.empty()) {
    buffer.resize(16, 94);
    dataBlock.push_back(buffer);
    buffer.clear();
  }
  return dataBlock;
}

string byte_to_string (vector<byte> buffer) {
  string str {};
  for (const auto& item : buffer) {
    if (item == 94) {break;}
    str += (char)item;
  }
  return str;
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
}
