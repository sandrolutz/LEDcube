/*
 * Project: LEDcube
 * Author:  Sandro Lutz
 * Email:   sandro.lutz@temparus.ch
 */

#include <avr/interrupt.h>
#include "global.h"
#include "button.h"
#include "effects.h"

#define BAUD_RATE 115200         // 57600 bps 115200 bps

// cube state buffer
#ifdef ARDUINO_X4
uint8_t cube[4][2];              // LAYER2__LAYER1
uint8_t tmpCube[4][2];
#elif ARDUINO_X8
uint8_t cube[8][8];              // [z][y][x]
uint8_t tmpCube[8][8];
#else
#error "Please specify cube size in Arduino configuration"
#endif

// Buttons
Button Button1;
#ifdef ARDUINO_X8
Button Button2;
#endif

extern uint8_t brightness;
uint8_t dimCounter;

// Counts through the layers (starting from 0)
uint8_t current_layer = LAYER_COUNT;

// Current state (See global.h)
uint8_t state = STATE_IDLE;
int8_t requestedState = -1;

struct SerialPacket {
    char data[50];
    int8_t length;
} receivePacket;
char lastReceivedByte;

uint8_t rawPacketCount;
bool serialConnected;
bool effectShouldFinish;

// Update state
void updateState(uint8_t value)
{
    if (value < 3) {
        effectShouldFinish = false;
        requestedState = -1;
        state = value;
        if (serialConnected) {
            Serial.print("STATE ");
            if (state == STATE_SERIAL) {
                Serial.println("SERIAL");
            } else if (state == STATE_EFFECTS) {
                Serial.println("EFFECTS");
            } else {
                Serial.println("IDLE");
            }
        }
    }
}

// Process received serial packet
void processSerialPacket()
{
    if (!strncmp(receivePacket.data, "HELLO", 5))               // Handshake
    {
        serialConnected = true;
        Serial.println("LEDcube v1.0");
        updateState(state);
    }
    else if (!strncmp(receivePacket.data, "STATE ", 6))         // change state
    {
        if (!strncmp(&receivePacket.data[6], "IDLE", 4)) {
            if (state == STATE_EFFECTS) {
                effectShouldFinish = true;
                requestedState = STATE_IDLE;
            } else {
                updateState(STATE_IDLE);
            }
        } else if (!strncmp(&receivePacket.data[6], "EFFECTS", 7)) {
            if (state != STATE_EFFECTS) {
                updateState(STATE_EFFECTS);
                startEffect(0);
            }
        } else if (!strncmp(&receivePacket.data[6], "SERIAL", 6)) {
            if (state == STATE_EFFECTS) {
                effectShouldFinish = true;
                requestedState = STATE_SERIAL;
            } else {
                updateState(STATE_SERIAL);
            }
        }
    }
    else if(!strncmp(receivePacket.data, "BRIGHTNESS ", 11) && state == STATE_SERIAL &&
            receivePacket.length == 12 && receivePacket.data[11] < MAX_BRIGHTNESS)
    {
        brightness = receivePacket.data[11];
    }
#ifdef ARDUINO_X4
    else if (!strncmp(receivePacket.data, "RAW", 3) && state == STATE_SERIAL &&
            receivePacket.length == 6 && receivePacket.data[3] < LAYER_COUNT)            // RAW data
    {
        tmpCube[receivePacket.data[3]][0] = receivePacket.data[4];
        tmpCube[receivePacket.data[3]][1] = receivePacket.data[5];
        ++rawPacketCount;
        if (rawPacketCount == LAYER_COUNT) {
            memcpy(cube, tmpCube, 8);
            rawPacketCount = 0;
        }
    }
#else // ARDUINO_X8
    else if (!strncmp(receivePacket.data, "RAW", 3) && state == STATE_SERIAL &&
            receivePacket.length == LAYER_COUNT+4 && receivePacket.data[3] < LAYER_COUNT) // RAW data
    {
        uint8_t i;
        for(i = 0; i < LAYER_COUNT; ++i) {
            tmpCube[receivePacket.data[3]][i] = receivePacket.data[4+i];
        }
        ++rawPacketCount;
        if (rawPacketCount == LAYER_COUNT) {
            memcpy(cube, tmpCube, 64);
            rawPacketCount = 0;
        }
    }
#endif
}

// Handles serial communication with the computer
void serialEvent()
{
    while (Serial.available()) {
        char data = Serial.read();
        if (lastReceivedByte == '\r' && data == '\n') {
            --receivePacket.length;
            processSerialPacket();
        } else {
            receivePacket.data[receivePacket.length++] = data;
        }
        lastReceivedByte = data;
    }
}


void setup()
{
    Serial.begin(BAUD_RATE);        // Open serial port

    // I/O-Port configuration
#ifdef ARDUINO_X4
    DDRD = (1<<PD6) | (1<<PD7);                       // Data output pins
    DDRB = (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3); // Level selector pins
    DDRC = (1<<PC0) | (1<<PC1);                       // CLK output pins
    PORTC = (1<<PC2);               // Pull-up resistors on button pins
#elif ARDUINO_X8
    DDRA = 0xFF;                    // Data output pins
    DDRC = 0xFF;                    // Level selector pins
    DDRD = (1<<PD6) | (1<<PD7);     // CLK output pins
    PORTB = (1<<PB0) | (1<<PB1);    // Pull-up resistors on button pins
#endif

    // Timer1
    // CTC (Mode 4) w/ prescaler 8
    TCCR1B = (1<<WGM12) | (1<<CS11);
    TIMSK |=  (1<<OCIE1A);
#ifdef ARDUINO_X4
    OCR1A = 400;    // compare match frequency = 4608 Hz -> LED update frequency = 1152 Hz
#else
    // ARDUINO_X8
    OCR1A = 200;    // compare match frequency = 9216 Hz -> LED update frequency = 1152 Hz
#endif
    sei();          // enable global interrupts

#ifdef ARDUINO_X4
    Button1.attach(18);
#elif ARDUINO_X8
    Button1.attach(0);
    Button2.attach(1);
#endif
}

void loop()
{
    Button1.update();
#ifdef ARDUINO_X8
    Button2.update();
#endif

    serialEvent();

    if (Button1.released()) {
        if (state == STATE_EFFECTS) {
            effectShouldFinish = true;
        } else {
            updateState(STATE_IDLE);
            startEffect(0);
        }
    }
#ifdef ARDUINO_X8
    if (Button2.released()) {
        effectShouldFinish = true;
        requestedState = STATE_IDLE;
    }
#endif

    processEffect(effectShouldFinish);

    if(isEffectFinished()) {
        if (requestedState == STATE_SERIAL) {
            updateState(STATE_SERIAL);
        } else if (requestedState == STATE_IDLE) {
            updateState(STATE_IDLE);
        } else if (state == STATE_EFFECTS) {
            if (getPreviousEffect()+1 == EFFECTS_COUNT) {
                startEffect(0);
            } else {
                startEffect(getPreviousEffect()+1);
            }
        } else if (requestedState == STATE_EFFECTS) {
            updateState(STATE_EFFECTS);
            startEffect(0);
        }
    }
}

// Interrupt routine on Timer1 compare match A
// Updates LED output
ISR(TIMER1_COMPA_vect) {
    // move on to the next layer
    if (++current_layer >= LAYER_COUNT) {
        current_layer = 0;
        if (++dimCounter > MAX_BRIGHTNESS) {
            dimCounter = 0;
        }
    }

    if (dimCounter <= brightness) {
        // Run through all the shift register values and send them
#ifdef ARDUINO_X4
        for (uint8_t i = 0; i < 2; ++i) {
            for (uint8_t j = 0; j < 8; j+=2) {
                // set cube state on data output pins
                PORTD = ((cube[current_layer][i] & (3<<j))<<(6-j));
                // update shift register
                __asm__("nop\n\t""nop\n\t");
                PORTC |= (1<<PC0);
                __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
                PORTC &= ~(1<<PC0);
            }
        }
        // Update LED output
        PORTC |= (1<<PC1);
        __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
        PORTC &= ~(1<<PC1);
#elif ARDUINO_X8
        for (uint8_t i = 0; i < 8; ++i) {
            // set cube state on data output pins
            PORTA = cube[current_layer][i];
            // update shift register
            __asm__("nop\n\t""nop\n\t");
            PORTD |= (1<<PD6);
            __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
            PORTD &= ~(1<<PD6);
        }
        // Update LED output
        PORTD |= (1<<PD7);
        __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
        PORTD &= ~(1<<PD7);
#endif

        // select new layer
        PORTC = (1 << current_layer);
    } else {
        PORTC = 0x00;
    }
};