#include <Arduino.h>
#include <Wire.h>

#define I2C_ADDR 0x3D
#define SDA_PIN 8
#define SCL_PIN 9
#define WIDTH 128
#define HEIGHT 64
#define PAGES 8

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

void set_pixel(uint8_t x, uint8_t y, uint8_t on)
{
    if (x >= WIDTH || y >= HEIGHT)
        return;
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    if (on)
        framebuffer[page][x] |= (1 << bit);
    else
        framebuffer[page][x] &= ~(1 << bit);
}

void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();
    set_pixel(0, 0, 1);
    set_pixel(127, 0, 1);
    set_pixel(0, 63, 1);
    set_pixel(127, 63, 1);
    display();
}

void loop() {}