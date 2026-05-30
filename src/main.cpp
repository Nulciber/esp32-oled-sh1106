#include <Arduino.h>
#include <Wire.h>
#include "sh1106.h"
#include "txt_eneide.h"
#include "txt_aveMaria.h"


#define SDA_PIN 8
#define SCL_PIN 9
#define TEXTEQUIDEFILE txt_aveMaria

void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();
     draw_char(10, 10, 0x61);    // a — dans police_standard.h
    draw_char(20, 10, 0x3B1);   // α — dans police_accents.h
    draw_char(30, 10, 0xE000);  // glyphe — dans police_accents.h
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