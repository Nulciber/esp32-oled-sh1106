#include <Arduino.h>
#include <Wire.h>
#include "caractere.h"
#include "police_standard.h"
#include "police_accents.h"
#include "eneide.h"
#include "aveMaria.h"

#define I2C_ADDR 0x3D
#define SDA_PIN 8
#define SCL_PIN 9
#define WIDTH 128
#define HEIGHT 64
#define PAGES 8
#define TEXTEQUIDEFILE aveMaria // Entrer après TEXTEQUIDEFILE le nom de la  variable qui contient le texte qui doit défiler à l'écran
// En bonne logique, la variable doit avoir le même nom que le fichier .h. Ce n'est pas obligatoire mais fortement recommandé

uint8_t framebuffer[PAGES][WIDTH];

void send_command(uint8_t cmd)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0x00);
    Wire.write(cmd);
    Wire.endTransmission();
}

void sh1106_init()
{
    send_command(0xAE);
    send_command(0xD5);
    send_command(0x80);
    send_command(0xA8);
    send_command(0x3F);
    send_command(0xD3);
    send_command(0x00);
    send_command(0x40);
    send_command(0x8D);
    send_command(0x14);
    send_command(0xA1);
    send_command(0xC8);
    send_command(0xDA);
    send_command(0x12);
    send_command(0x81);
    send_command(0xFF);
    send_command(0xD9);
    send_command(0x22);
    send_command(0xDB);
    send_command(0x35);
    send_command(0xA4);
    send_command(0xA6);
    send_command(0xAF);
}

void clear()
{
    memset(framebuffer, 0x00, sizeof(framebuffer));
}

void display()
{
    for (uint8_t page = 0; page < PAGES; page++)
    {
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(0x00);
        Wire.write(0xB0 + page);
        Wire.write(0x00);
        Wire.write(0x10);
        Wire.endTransmission();

        Wire.beginTransmission(I2C_ADDR);
        Wire.write(0x40);
        Wire.write(framebuffer[page], WIDTH / 2);
        Wire.endTransmission();

        Wire.beginTransmission(I2C_ADDR);
        Wire.write(0x40);
        Wire.write(framebuffer[page] + WIDTH / 2, WIDTH / 2);
        Wire.endTransmission();
    }
}

void set_pixel(int x, int y, uint8_t on)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    if (on)
        framebuffer[page][x] |= (1 << bit);
    else
        framebuffer[page][x] &= ~(1 << bit);
}

void draw_hline(uint8_t x0, uint8_t x1, uint8_t y)
{
    for (uint8_t x = x0; x <= x1; x++)
        set_pixel(x, y, 1);
}

void draw_vline(uint8_t x, uint8_t y0, uint8_t y1)
{
    for (uint8_t y = y0; y <= y1; y++)
        set_pixel(x, y, 1);
}

void draw_rectangle(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1)
{
    draw_hline(x0, x1, y0);
    draw_vline(x1, y0, y1);
    draw_hline(x0, x1, y1);
    draw_vline(x0, y0, y1);
}

void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx = (dx > 0) ? 1 : -1; // direction en X
    int sy = (dy > 0) ? 1 : -1; // direction en Y
    dx = abs(dx);
    dy = abs(dy);
    int err = dx - dy;

    while (1)
    {
        set_pixel(x0, y0, 1);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_circle(uint8_t cx, uint8_t cy, uint8_t r)
{
    int x = 0;
    int y = r;
    int err = 3 - 2 * r;

    while (x <= y)
    {
        set_pixel(cx + x, cy + y, 1);
        set_pixel(cx - x, cy + y, 1);
        set_pixel(cx + x, cy - y, 1);
        set_pixel(cx - x, cy - y, 1);
        set_pixel(cx + y, cy + x, 1);
        set_pixel(cx - y, cy + x, 1);
        set_pixel(cx + y, cy - x, 1);
        set_pixel(cx - y, cy - x, 1);

        if (err < 0)
        {
            err += 4 * x + 6;
        }
        else
        {
            err += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void draw_glyph(int x, int y, Caractere car)
{
    for (int ligne = 0; ligne < 8; ligne++)
    {
        uint8_t octet = car.bitmap[ligne];
        for (int col = 0; col < 8; col++)
        {
            if (octet & (0x80 >> col))
            {
                set_pixel(x + col, y + ligne, 1);
            }
        }
    }
}

void draw_char(int x, int y, uint8_t c)
{
    for (int i = 0; i < sizeof(police_standard) / sizeof(Caractere); i++)
    {
        if (police_standard[i].code == c)
        {
            draw_glyph(x, y, police_standard[i]);
            return;
        }
    }
    for (int i = 0; i < sizeof(police_accents) / sizeof(Caractere); i++)
    {
        if (police_accents[i].code == c)
        {
            draw_glyph(x, y, police_accents[i]);
            return;
        }
    }
}

// Affiche une chaîne de caractères à la position (x, y)
// str : pointeur sur le premier caractère de la chaîne (se termine par \0)
// Chaque caractère fait 8 pixels de large
void draw_string(int x, int y, const char *str)
{
    while (*str)
    {                          // tant qu'on n'est pas à la fin de la chaîne (\0)
        draw_char(x, y, *str); // on dessine le caractère courant
        x += 8;                // on avance de 8 pixels
        str++;                 // on passe au caractère suivant
    }
}
void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();
}



void loop()
{
    int longueur = strlen(TEXTEQUIDEFILE) * 8;
    static int i = 0;
    
    clear();
    draw_string(WIDTH - i, 28, TEXTEQUIDEFILE);
    if (i > longueur - WIDTH)
        draw_string(WIDTH - i + longueur, 28, TEXTEQUIDEFILE);
    display();
    delay(1);
    i++;
    if (i >= longueur) i = 0;
}