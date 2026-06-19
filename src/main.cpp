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
    
}
void loop()
{
    clear();
    display();
    draw_circle(63, 31, 30);
    draw_line(63, 2, 63, 60);
    draw_line(40, 48, 63, 33);
    draw_line(87, 48, 63, 33);
    display();
    delay(500);
    clear();
    display();
    delay(250);
}