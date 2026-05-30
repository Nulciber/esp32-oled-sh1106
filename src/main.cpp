#include <Arduino.h>
#include <Wire.h>
#include "sh1106.h"
#include "eneide.h"
#include "aveMaria.h"


#define SDA_PIN 8
#define SCL_PIN 9
#define TEXTEQUIDEFILE aveMaria

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