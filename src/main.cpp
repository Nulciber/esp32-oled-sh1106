#include <Arduino.h>
#include <Wire.h>
#include "sh1106.h"
#include "buzzer.h"
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
    buzzer_init();
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
    // buzzer_bip(1200);
    // delay(500);
    // clear();
    // display();
    // delay(500);
    buzzer_note(A3, CROCHE);
    buzzer_note(A3, CROCHE);
    buzzer_note(A3, CROCHE);
    buzzer_note(F3, NOIRE);
    buzzer_note(F3, CROCHE);
    buzzer_note(E3, NOIRE_POINTEE);
    buzzer_note(E3, CROCHE);
    buzzer_note(D3, CROCHE);
    buzzer_note(E3, CROCHE);
    buzzer_note(F3, NOIRE_POINTEE);
    buzzer_note(F3, NOIRE_POINTEE);
    
    buzzer_note(E3, CROCHE);
    buzzer_note(F3, CROCHE);
    buzzer_note(G3, CROCHE);
    buzzer_note(G3, CROCHE);
    buzzer_note(F3, CROCHE);
    buzzer_note(G3, CROCHE);
    buzzer_note(A3, BLANCHE_POINTEE);
}