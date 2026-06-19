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
void buzzer_note(int frequence, int duree_ms);

// Notes (octave 3, La3 = 440Hz)
#define C3  262
#define CS3 277
#define D3  294
#define DS3 311
#define E3  330
#define F3  349
#define FS3 370
#define G3  392
#define GS3 415
#define A3  440
#define AS3 466
#define B3  494
#define C4  523

// Tempo et durées (en ms)
#define BPM 120
#define NOIRE   (60000 / BPM)
#define BLANCHE (NOIRE * 2)
#define RONDE   (NOIRE * 4)
#define CROCHE  (NOIRE / 2) 
#define CROCHE_POINTEE  (CROCHE * 1.5)
#define NOIRE_POINTEE   (NOIRE * 1.5)
#define BLANCHE_POINTEE (BLANCHE * 1.5)