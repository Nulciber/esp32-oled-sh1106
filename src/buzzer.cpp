#include "buzzer.h"

void buzzer_init() {
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
}

void buzzer_on() {
    ledcWrite(BUZZER_CHANNEL, 512); // env.50% duty cycle (512/1024). Sert à allumer le buzzer. 512 n'a pas d'influence
}

void buzzer_off() {
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_bip(int duree_ms) {
    buzzer_on();
    delay(duree_ms);
    buzzer_off();
}
void buzzer_note(int frequence, int duree_ms) {
    ledcWriteTone(BUZZER_CHANNEL, frequence);
    delay(duree_ms);
    ledcWriteTone(BUZZER_CHANNEL, 0);
}