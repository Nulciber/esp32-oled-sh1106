#include <Arduino.h>
#include <Wire.h>
#include "sh1106.h"
#include "txt_eneide.h"
#include "txt_aveMaria.h"

#define SDA_PIN 8
#define SCL_PIN 9
#define TEXTEQUIDEFILE txt_eneide

void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();
    draw_char(0, 10, 0x03B1);  // α
    draw_char(10, 10, 0x03B2); // β
    draw_char(20, 10, 0x03C9); // Ω
    display();
    display();
}

void loop()
{
}