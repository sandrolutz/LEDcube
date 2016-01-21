/*
 * Project: LEDcube
 * Author:  Sandro Lutz
 * Email:   sandro.lutz@temparus.ch
 */

#include "effects.h"
#include "global.h"
#include "utils.h"
#include "draw.h"

#define NO_EFFECT_ACTIVE 0xFF

uint8_t brightness = MAX_BRIGHTNESS;

uint8_t previousEffectIndex = NO_EFFECT_ACTIVE;
uint8_t currentEffectIndex = NO_EFFECT_ACTIVE;
uint16_t effectState[5];            // can be used by every effect
unsigned long lastExecutionTime;

// Start a new effect.
// NOTE: This will force finish the current effect!
void startEffect(uint8_t index)
{
    if (index < EFFECTS_COUNT) {
        currentEffectIndex = index;
    }
    fill(0x00);
    brightness = MAX_BRIGHTNESS;
}

// Get current effect index
uint8_t getCurrentEffect()
{
    return currentEffectIndex;
}

// Get previous effect index
uint8_t getPreviousEffect()
{
    return previousEffectIndex;
}

// Checks if the current effect has been finished
bool isEffectFinished()
{
    return currentEffectIndex == NO_EFFECT_ACTIVE;
}

// Finish effect right now. This may look strange
void forceFinishEffect()
{
    fill(0x00);
    memset(effectState, 0x00, 10);            // reset effect memory
    previousEffectIndex = currentEffectIndex;
    currentEffectIndex = NO_EFFECT_ACTIVE;
}

// Executes another cycle of the effect
void processEffect(bool shouldFinish)
{
    unsigned long deltaTime = getTimeDifference(lastExecutionTime, millis());

    if (currentEffectIndex == 0)                    // rain effect
    {
        if (deltaTime >= 1000)
        {
            shift(AXIS_Z, -1);
            if(!shouldFinish) {
                uint8_t random_number = rand() % 4;

                while (random_number--) {
                    setVoxel(rand() % LAYER_COUNT, rand() % LAYER_COUNT, LAYER_COUNT - 1);
                }
            } else {
                ++effectState[0];
            }

            if (effectState[0] == LAYER_COUNT) {
                forceFinishEffect();
            }
            lastExecutionTime = millis();
        }
    }
    else if (currentEffectIndex == 1)               // toggle random voxel
    {
        if (deltaTime >= 500)
        {
            uint8_t random_number = rand() % LAYER_COUNT;

            while (random_number--) {
                toggleVoxel(rand() % LAYER_COUNT, rand() % LAYER_COUNT, rand() % LAYER_COUNT);
            }
            lastExecutionTime = millis();
        }
    }
    else if (currentEffectIndex == 2)               // Plane bounce
    {
        // STATE:: 0=axis; 1=position; 2=counter
        if (effectState[0] == 0) {
            effectState[0] = AXIS_Z;
        }

        if (deltaTime >= 400)
        {
            fill(0x00);
            if (effectState[0] == AXIS_Z) {
                setPlaneZ(effectState[1]);
            } else if (effectState[0] == AXIS_Y) {
                setPlaneY(effectState[1]);
            } else {
                setPlaneX(effectState[1]);
            }

            if ((effectState[1] == 0 || effectState[1] == LAYER_COUNT - 1) && shouldFinish) {
                forceFinishEffect();
                return;
            }

            if (effectState[1] == 0 && effectState[2] == 4) {
                if (effectState[0] == AXIS_Z) {
                    effectState[0] = AXIS_Y;
                    effectState[2] = 0;
                } else if (effectState[0] == AXIS_Y) {
                    effectState[0] = AXIS_X;
                    effectState[2] = 0;
                } else if (effectState[0] == AXIS_X) {
                    effectState[0] = AXIS_Z;
                    effectState[2] = 0;
                }
                lastExecutionTime = millis();
                return;
            }

            if (effectState[2] % 2 == 0) {
                if (++effectState[1] == LAYER_COUNT-1) {
                    ++effectState[2];
                }
            } else {
                if (--effectState[1] == 0) {
                    ++effectState[2];
                }
            }

            lastExecutionTime = millis();
        }
    }
    else if (currentEffectIndex == 3)               // sticky plane bounce
    {
        // STATE:: 0=axis; 1=position; 2=counter
        if (effectState[0] == 0) {
            effectState[0] = AXIS_Z;
        }

        if (deltaTime >= 400)
        {
            if (effectState[2] == 0) {
                if (effectState[0] == AXIS_Z) {
                    setPlaneZ(effectState[1]);
                } else if (effectState[0] == AXIS_Y) {
                    setPlaneY(effectState[1]);
                } else {
                    setPlaneX(effectState[1]);
                }
            } else {
                if (effectState[0] == AXIS_Z) {
                    clrPlaneZ(effectState[1]);
                } else if (effectState[0] == AXIS_Y) {
                    clrPlaneY(effectState[1]);
                } else {
                    clrPlaneX(effectState[1]);
                }
            }

            if (effectState[1] == LAYER_COUNT - 1 && effectState[2] == 1 && shouldFinish) {
                forceFinishEffect();
                return;
            }

            if (effectState[1] == LAYER_COUNT-1 && effectState[2] == 1) {
                if (effectState[0] == AXIS_Z) {
                    effectState[0] = AXIS_Y;
                    effectState[1] = 0;
                    effectState[2] = 0;
                } else if (effectState[0] == AXIS_Y) {
                    effectState[0] = AXIS_X;
                    effectState[1] = 0;
                    effectState[2] = 0;
                } else if (effectState[0] == AXIS_X) {
                    effectState[0] = AXIS_Z;
                    effectState[1] = 0;
                    effectState[2] = 0;
                }
                lastExecutionTime = millis();
                return;
            }

            if(++effectState[1] == LAYER_COUNT) {
                effectState[1] = 0;
                ++effectState[2];
            }

            lastExecutionTime = millis();
        }
    }
    else if (currentEffectIndex == 4)               // Blink
    {
        // STATE: 0=variable delay; 1=LED state; 2=accelerating/slowing down
        if (effectState[0] == 0) {
            effectState[0] = 750;
        }
        if (effectState[1] == 0 && ((effectState[2] == 0 && deltaTime >= effectState[0]) || deltaTime >= 751 - effectState[0])) {
            if (effectState[0] == 0) {
                effectState[0] = 750;

                if (effectState[2] == 0) {
                    effectState[2] = 1;
                } else {
                    if (shouldFinish) {
                        forceFinishEffect();
                        return;
                    }
                    effectState[2] = 0;
                }
            }
            effectState[1] = 1;
            fill(0xFF);
            lastExecutionTime = millis();
        } else if (deltaTime >= 100 && effectState[1] == 1) {
            fill(0x00);
            effectState[0] = effectState[0] - (15+(1000/(effectState[0]/10)));
            effectState[1] = 0;
            lastExecutionTime = millis();
        }
    }
    // TODO: add more effects
    // TODO: add text functions to ARDUINO_X8 boards
}