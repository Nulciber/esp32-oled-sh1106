// sh1106.h :  Déclarations et constantes
#ifndef SH1106_H
#define SH1106_H

#include <Arduino.h>
#include "caractere.h"

#define I2C_ADDR 0x3D
#define WIDTH 128
#define HEIGHT 64
#define PAGES 8


void send_command(uint8_t cmd);
void sh1106_init();
void clear();
void display();
void set_pixel(int x, int y, uint8_t on);
void draw_hline(uint8_t x0, uint8_t x1, uint8_t y);
void draw_vline(uint8_t x, uint8_t y0, uint8_t y1);
void draw_rectangle(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);
void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void draw_circle(uint8_t cx, uint8_t cy, uint8_t r);
void draw_glyph(int x, int y, Caractere car);
void draw_char(int x, int y, uint16_t c);
void draw_string(int x, int y, const char *str);

#endif