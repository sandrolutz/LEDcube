/*
 * Project: LEDcube
 * Author:  Sandro Lutz
 * Email:   sandro.lutz@temparus.ch
 */

#ifndef LEDCUBE_GLOBAL_H
#define LEDCUBE_GLOBAL_H

#define STATE_IDLE 0
#define STATE_EFFECTS 1
#define STATE_SERIAL 2

#ifdef ARDUINO_X4
#define LAYER_COUNT 4
#elif ARDUINO_X8
#define LAYER_COUNT 8
#else
#error "Please specify cube size in Arduino configuration"
#endif

#endif