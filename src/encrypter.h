
#ifndef ENCRYPTER_H
#define ENCRYPTER_H

#include "bmp.h"
#include "string"

extern "C" void encode_bits(int& pixelAddress, int& charAdress);

extern "C" void decode_bits(int& pixelAddress, int& charAdress);

void encrypt_message(BMP& image, std::string msg);

char* decrypt_message(BMP& image);

#endif 
