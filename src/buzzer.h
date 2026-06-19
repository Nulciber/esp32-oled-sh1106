#pragma once
#include <Arduino.h>

#define BUZZER_PIN 4
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 440
#define BUZZER_RESOLUTION 10

void buzzer_init();
void buzzer_on();
void buzzer_off();
void buzzer_bip(int duree_ms);