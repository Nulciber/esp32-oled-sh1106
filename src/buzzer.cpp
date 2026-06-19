#include "buzzer.h"

void buzzer_init() {
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
}

void buzzer_on() {
    ledcWrite(BUZZER_CHANNEL, 100); // 50% duty cycle
}

void buzzer_off() {
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_bip(int duree_ms) {
    buzzer_on();
    delay(duree_ms);
    buzzer_off();
}