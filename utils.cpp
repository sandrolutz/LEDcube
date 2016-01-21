/*
 * Project: LEDcube
 * Author:  Sandro Lutz
 * Email:   sandro.lutz@temparus.ch
 */

#include "Utils.h"

unsigned long getTimeDifference(unsigned long time1, unsigned long time2)
{
    if(time1 > time2) { // an overflow occured
        return 4294967295 - time1 + time2;
    }
    return time2 - time1;
}