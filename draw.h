/*
 * Project: LEDcube
 * Author:  Sandro Lutz
 * Email:   sandro.lutz@temparus.ch
 */

#ifndef LEDCUBE_DRAW_H
#define LEDCUBE_DRAW_H

#include <Arduino.h>

#define AXIS_X 1
#define AXIS_Y 2
#define AXIS_Z 3

// defines box type
#define BOX_FILLED 1
#define BOX_WALLS  2
#define BOX_FRAME  3

// ---------------------------------------------------------------------------------------
// Draw functions for LEDcube
// ---------------------------------------------------------------------------------------

// Turn on a single voxel
void setVoxel(uint8_t x, uint8_t y, uint8_t z);
// Turn off a single voxel
void clrVoxel(uint8_t x, uint8_t y, uint8_t z);
// Alter a single voxel base on the the state
void alterVoxel(uint8_t x, uint8_t y, uint8_t z, uint8_t state);
// Toggle a single voxel
void toggleVoxel(uint8_t x, uint8_t y, uint8_t z);
// Get the current status of a voxel
uint8_t getVoxel(uint8_t x, uint8_t y, uint8_t z);


// Fill the whole buffer with a pattern
// Special: fill(0x00) -> clear
//          fill(0xff) -> fill all
void fill(uint8_t pattern);


// Set all voxels along a Y/Z plane at a given point on axis X
void setPlaneX(uint8_t x);
void clrPlaneX(uint8_t x);

// Set all voxels along a X/Z plane at a given point on axis Y
void setPlaneY(uint8_t y);
void clrPlaneY(uint8_t y);

// Set all voxels along a X/Y plane at a given point on axis Z
void setPlaneZ(uint8_t z);
void clrPlaneZ(uint8_t z);

// Draw a box
// Type: BOX_FILLED Draws a box with all walls drawn and all voxels inside set
//       BOX_WALLS  Draws a box with all walls drawn. The state of all voxels inside won't be changed.
//       BOX_FRAME  This only draws the corners and edges. No walls will be drawn.
void box(uint8_t type, uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2);

// Draws a line between two coordinates in 3D space
void line(uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2);

// Shift the entire content of the cube along an axis
void shift(uint8_t axis, int8_t direction);


// ---------------------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------------------

// Order two values in ascending order (smaller one first)
void orderValues(uint8_t *value1, uint8_t *value2);

// Returns a byte with a row of 1's drawn in it.
// Example: byteline(2,5) returns 0b00111100
uint8_t byteline(uint8_t start, uint8_t end);

// Reverses a byte
uint8_t bitswap(uint8_t value);

#endif