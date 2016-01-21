/*
 * Project: LEDcube
 * Author:  Sandro Lutz
 * Email:   sandro.lutz@temparus.ch
 */

#include "draw.h"

#ifdef ARDUINO_X4
#define LAYER_COUNT 4
extern uint8_t cube[4][2];              // LAYER2__LAYER1
#elif ARDUINO_X8
#define LAYER_COUNT 8
extern uint8_t cube[8][8];              // [z][y][x]
#else
#error "Please specify cube size in Arduino configuration"
#endif

// ---------------------------------------------------------------------------------------
// Draw functions for LEDcube
// ---------------------------------------------------------------------------------------

// This function checks if the coordinate is valid
// Because using uint8_t, only the upper limit needs to be checked.
bool inRange(uint8_t x, uint8_t y, uint8_t z)
{
    return x < LAYER_COUNT && y < LAYER_COUNT && z < LAYER_COUNT;
}

// Turn on a single voxel
void setVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    if (inRange(x,y,z)) {
#ifdef ARDUINO_X4
        cube[z][y/2] |= (1<<(x+(y%2)*4));
#elif ARDUINO_X8
        cube[z][y] |= (1<<x);
#endif
    }
}

// Turn off a single voxel
void clrVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    if (inRange(x,y,z)) {
#ifdef ARDUINO_X4
        cube[z][y/2] &= ~(1<<(x+(y%2)*4));
#elif ARDUINO_X8
        cube[z][y] &= ~(1<<x);
#endif
    }
}

// Alter a single voxel base on the the state
void alterVoxel(uint8_t x, uint8_t y, uint8_t z, uint8_t state)
{
    if (state == 0x00) {
        clrVoxel(x,y,z);
    } else {
        setVoxel(x,y,z);
    }
}

// Toggle a single voxel
void toggleVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    if (inRange(x,y,z)) {
#ifdef ARDUINO_X4
        cube[z][y/2] ^= (1<<(x+(y%2)*4));
#elif ARDUINO_X8
        cube[z][y] ^= (1 << x);
#endif
    }
}

// Get the current status of a voxel
uint8_t getVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    if (inRange(x,y,z)) {
#ifdef ARDUINO_X4
        if (cube[z][y/2] & (1<<(x+(y%2)*4))) {
            return 0x01;
        }
        return 0x00;
#elif ARDUINO_X8
        if (cube[z][y] & (1<<x)) {
            return 0x01;
        }
        return 0x00;
#endif
    }
    return 0x00;
}

// Fill the whole buffer with a pattern
// Special: fill(0x00) -> clear
//          fill(0xff) -> fill all
void fill(uint8_t pattern)
{
    uint8_t z;
    uint8_t y;
    for (z=0; z<LAYER_COUNT; ++z)
    {
#ifdef ARDUINO_X4
        for (y=0; y<2; ++y)
        {
            cube[z][y] = pattern;
        }
#else
        for (y=0; y<LAYER_COUNT; ++y)
        {
            cube[z][y] = pattern;
        }
#endif
    }
}

// Set all voxels along a Y/Z plane at a given point on axis X
void setPlaneX(uint8_t x)
{
    uint8_t z;
    uint8_t y;
    if (inRange(x,0,0))
    {
        for (z=0; z<LAYER_COUNT; ++z)
        {
#ifdef ARDUINO_X4
            for (y=0; y<2; ++y)
            {
                cube[z][y] |= (1 << x) | (1 << (x+4));
            }
#else
            for (y=0; y<LAYER_COUNT; ++y)
            {
                cube[z][y] |= (1 << x);
            }
#endif
        }
    }
}

// Clear voxels in the same manner as above
void clrPlaneX(uint8_t x)
{
    uint8_t z;
    uint8_t y;
    if (inRange(x,0,0))
    {
        for (z=0; z<LAYER_COUNT; ++z)
        {
#ifdef ARDUINO_X4
            for (y=0; y<2; ++y)
            {
                cube[z][y] &= ~((1 << x) | (1 << (x+4)));
            }
#else
            for (y=0; y<LAYER_COUNT; ++y)
            {
                cube[z][y] &= ~(1 << x);
            }
#endif
        }
    }
}

// Set all voxels along a X/Z plane at a given point on axis Y
void setPlaneY(uint8_t y)
{
    uint8_t z;
    if (inRange(0,y,0))
    {
        for (z=0; z<LAYER_COUNT; ++z) {
#ifdef ARDUINO_X4
            cube[z][y/2] |= (15<<((y%2)*4));
#else
            cube[z][y] = 0xFF;
#endif
        }
    }
}

// Clear voxels in the same manner as above
void clrPlaneY(uint8_t y)
{
    uint8_t z;
    if (inRange(0,y,0))
    {
        for (z=0; z<LAYER_COUNT; ++z)
#ifdef ARDUINO_X4
            cube[z][y/2] &= ~(15<<((y%2)*4));
#else
                cube[z][y] = 0x00;
#endif
    }
}

// Set all voxels along a X/Y plane at a given point on axis Z
void setPlaneZ(uint8_t z)
{
    uint8_t i;
    if (inRange(0,0,z))
    {
        for (i=0; i<LAYER_COUNT; ++i) {
            cube[z][i] = 0xff;
        }
    }
}

// Clear voxels in the same manner as above
void clrPlaneZ(uint8_t z)
{
    uint8_t i;
    if (inRange(0,0,z))
    {
        for (i=0; i<LAYER_COUNT; ++i)
            cube[z][i] = 0x00;
    }
}

// Draw a box
// Type: BOX_FILLED Draws a box with all walls drawn and all voxels inside set
//       BOX_WALLS  Draws a box with all walls drawn. The state of all voxels inside won't be changed.
//       BOX_FRAME  This only draws the corners and edges. No walls will be drawn.
void box(uint8_t type, uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2)
{
    uint8_t i;
    uint8_t j;

    if (inRange(x1,y1,z1) && inRange(x2,y2,z2)) {
        orderValues(&x1, &x2);
        orderValues(&y1, &y2);
        orderValues(&z1, &z2);

        if (type == BOX_FILLED)
        {
            for (i = z1; i <= z2; ++i) {
                for (j = y1; j <= y2; ++j) {
#ifdef ARDUINO_X4
                    cube[i][j/2] |= (byteline(x1, x2)<<((j%2)*4));
#else
                    cube[i][j] |= byteline(x1, x2);
#endif
                }
            }
        }
        else if (type == BOX_WALLS)
        {
            for (i = z1; i <= z2; ++i) {
                for (j = y1; j <= y2; ++j) {
                    if (j == y1 || j == y2 || i == z1 || i == z2) {
#ifdef ARDUINO_X4
                        cube[i][j/2] = (byteline(x1, x2)<<((j%2*4)));
#else
                        cube[i][j] = byteline(x1, x2);
#endif
                    } else {
#ifdef ARDUINO_X4
                        cube[i][j/2] |= (1 << (x1+(j%2)*4)) | (1 << (x2+(j%2)*4));
#else
                        cube[i][j] |= (1 << x1) | (1 << x2);
#endif
                    }
                }
            }
        }
        else if (type == BOX_FRAME)
        {
            // Lines along X axis
#ifdef ARDUINO_X4
            cube[z1][y1/2] = (byteline(x1, x2) << ((y1%2)*4));
            cube[z1][y2/2] = (byteline(x1, x2) << ((y2%2)*4));
            cube[z2][y1/2] = (byteline(x1, x2) << ((y1%2)*4));
            cube[z2][y2/2] = (byteline(x1, x2) << ((y2%2)*4));
#else
            cube[z1][y1] = byteline(x1, x2);
            cube[z1][y2] = byteline(x1, x2);
            cube[z2][y1] = byteline(x1, x2);
            cube[z2][y2] = byteline(x1, x2);
#endif

            // Lines along Y axis
            for (j = y1; j <= y2; ++j) {
                setVoxel(x1, j, z1);
                setVoxel(x1, j, z2);
                setVoxel(x2, j, z1);
                setVoxel(x2, j, z2);
            }

            // Lines along Z axis
            for (i = z1; i <= z2; ++i) {
                setVoxel(x1, y1, i);
                setVoxel(x1, y2, i);
                setVoxel(x2, y1, i);
                setVoxel(x2, y2, i);
            }
        }
    }

}

// Draws a line between two coordinates in 3D space
void line(uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2)
{
    // TODO: optimize this function (without using float!)
    float xy;       // how many voxels do we move on the y axis for each step on the x axis
    float xz;       // how many voxels do we move on the y axis for each step on the x axis
    uint8_t x,y,z;

    // We always want to draw the line from x=0 to x=7.
    // If x1 is bigget than x2, we need to flip all the values.
    if (x1 > x2) {
        int tmp;
        tmp = x2;
        x2 = x1;
        x1 = tmp;
        tmp = y2;
        y2 = y1;
        y1 = tmp;
        tmp = z2;
        z2 = z1;
        z1 = tmp;
    }

    if (y1 > y2) {
        xy = (float) (y1 - y2) / (float) (x2 - x1);
    } else {
        xy = (float) (y2 - y1) / (float) (x2 - x1);
    }

    if (z1 > z2) {
        xz = (float) (z1 - z2) / (float) (x2 - x1);
    } else {
        xz = (float) (z2 - z1) / (float) (x2 - x1);
    }

    // For each step of x, y increments by:
    for (x = x1; x <= x2; ++x) {
        y = (xy * (x - x1)) + y1;
        z = (xz * (x - x1)) + z1;
        setVoxel(x, y, z);
    }
}

// Shift the entire content of the cube along an axis
void shift(uint8_t axis, int8_t direction)
{
    uint8_t i, j ,k;
    uint8_t current_pos, previous_pos;
    uint8_t state = 0x00;
    uint8_t tmp;

    for (i = 0; i < LAYER_COUNT-1; ++i)
    {
        if (direction == -1) {
            current_pos = i;
        } else {
            current_pos = (7-i);
        }

        for (j = 0; j < LAYER_COUNT; ++j)
        {
            for (k = 0; k < LAYER_COUNT; ++k)
            {
                if (direction == -1) {
                    previous_pos = current_pos+1;
                } else {
                    previous_pos = current_pos-1;
                }

                if (axis == AXIS_Z) {
                    state = getVoxel(j,k,previous_pos);
                    alterVoxel(j,k,current_pos,state);
                } else if (axis == AXIS_Y) {
                    state = getVoxel(j,previous_pos,k);
                    alterVoxel(j,current_pos,k,state);
                } else if (axis == AXIS_X) {
                    state = getVoxel(previous_pos,k,j);
                    alterVoxel(current_pos,k,j,state);
                }
            }
        }
    }

    if (direction == -1) {
        current_pos = 7;
    } else {
        current_pos = 0;
    }

    if (axis == AXIS_X) {
        clrPlaneX(current_pos);
    } else if (axis == AXIS_Y) {
        clrPlaneY(current_pos);
    } else if(axis == AXIS_Z) {
        clrPlaneZ(current_pos);
    }
}

// ---------------------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------------------

// Order two values in ascending order (smaller one first)
void orderValues(uint8_t *value1, uint8_t *value2)
{
    if (*value1>*value2)
    {
        uint8_t tmp;
        tmp = *value1;
        *value1 = *value2;
        *value2 = tmp;
    }
}

// Returns a byte with a row of 1's drawn in it.
// byteline(2,5) gives 0b00111100
uint8_t byteline(uint8_t start, uint8_t end)
{
    return ((0xff<<start) & ~(0xff<<(end+1)));
}

// Reverses a byte
uint8_t bitswap(uint8_t value)
{
    uint8_t result;

    asm("mov __tmp_reg__, %[in] \n\t"
            "lsl __tmp_reg__  \n\t"   /* shift out high bit to carry */
            "ror %[out] \n\t"  /* rotate carry __tmp_reg__to low bit (eventually) */
            "lsl __tmp_reg__  \n\t"   /* 2 */
            "ror %[out] \n\t"
            "lsl __tmp_reg__  \n\t"   /* 3 */
            "ror %[out] \n\t"
            "lsl __tmp_reg__  \n\t"   /* 4 */
            "ror %[out] \n\t"

            "lsl __tmp_reg__  \n\t"   /* 5 */
            "ror %[out] \n\t"
            "lsl __tmp_reg__  \n\t"   /* 6 */
            "ror %[out] \n\t"
            "lsl __tmp_reg__  \n\t"   /* 7 */
            "ror %[out] \n\t"
            "lsl __tmp_reg__  \n\t"   /* 8 */
            "ror %[out] \n\t"
    : [out] "=r" (result) : [in] "r" (value));
    return(result);
}
