#ifndef READ_WRITE_H
#define READ_WRITE_H

#define RST_PIN         24
#define SS_PIN          37

#include "globalinclude.h"

using namespace std;

extern MFRC522 mfrc522;

void mfrc_setup();
string parsePICCMemory();
string byte_to_string (vector<byte> buffer);
vector<vector<byte>> string_to_bytes (const char* str);
void authenticate_picc (uint8_t trailerBlock);
void write_to_picc (uint8_t blockAddr, uint8_t* data);
vector<byte> read_from_picc (uint8_t blockAddr);
void dump_byte_array(byte *buffer, byte bufferSize);

#endif
