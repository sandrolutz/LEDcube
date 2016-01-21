/*
 * Project: LEDcube
 * Author:  Sandro Lutz
 * Email:   sandro.lutz@temparus.ch
 */

#ifndef LEDCUBE_EFFECTS_H
#define LEDCUBE_EFFECTS_H

#include <Arduino.h>

#define EFFECTS_COUNT 5
#define MAX_BRIGHTNESS 10

void startEffect(uint8_t index);
uint8_t getCurrentEffect();
uint8_t getPreviousEffect();
bool isEffectFinished();

void processEffect(bool shouldFinish);
void forceFinishEffect();

#endif