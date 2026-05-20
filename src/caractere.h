#ifndef CARACTERE_H
#define CARACTERE_H
#include <Arduino.h>

struct Caractere {
    uint8_t code;      // Code ASCII du glyphe
    uint8_t bitmap[8]; // Bitmap du glyphe (8x8)
};

#endif