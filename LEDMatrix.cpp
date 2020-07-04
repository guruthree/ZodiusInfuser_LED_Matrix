#include "LEDMatrix.h"

#ifndef TEENSYDUINO
void delayNanoseconds(uint8_t c) {
    for (uint8_t i = 0; i < c; i++) {
    }
}
#endif

LEDMatrix::LEDMatrix(uint16_t w, uint16_t h, SPIClass *spiClass, 
        uint8_t rowClock, uint8_t rowLatch, uint8_t rowData, uint8_t colLatch, uint8_t bl)
        : GFXcanvas8(w, h) {
    buffer2 = (uint8_t*)malloc(w * h);
    memset(buffer2, 0, w * h);

    spi = spiClass;

    row_clock = rowClock;
    row_latch = rowLatch;
    row_data = rowData;
    col_latch = colLatch;

    brightAt = 0;
    brightnessLevels = bl;
}

void LEDMatrix::begin() {
    pinMode(row_clock, OUTPUT);
    pinMode(row_latch, OUTPUT);
    pinMode(row_data, OUTPUT);
    pinMode(col_latch, OUTPUT);

    clearDisplay();
}

void LEDMatrix::flip() {
    // note, not actually a flip because buffer is private in GFXcanvas8

    noInterrupts();
    memcpy(buffer2, getBuffer(), WIDTH * HEIGHT);
    interrupts();
}

void LEDMatrix::display() {
    // print out buffer2

    for (int whichRow = 0; whichRow < HEIGHT; whichRow++) {

        // set latch low so updating data doesn't get written out immediately
        digitalWriteFast(row_latch, 0);
        digitalWriteFast(col_latch, 0);
        
        // select which row
        for (int d = 0; d < HEIGHT; d++) {
            digitalWriteFast(row_clock, 0);

            if (d == whichRow) {
                digitalWriteFast(row_data, 1);
            }
            else {
                digitalWriteFast(row_data, 0);
            }

            digitalWriteFast(row_clock, 1);
            delayNanoseconds(50);
        }

        // write out the data for that row
        unsigned short int mydata;
        for (int i = WIDTH/8 - 1; i >= 0; i--) {
            mydata = 0;
            for (int j = 0; j < 8; j++) {
                if (buffer2[(7-whichRow)*WIDTH + i*8+j] > brightAt) {
                    bitSet(mydata,j);
                }
            }
            spi->transfer(mydata);
        }

        // re-enable latches
        digitalWriteFast(row_latch, 1);
        digitalWriteFast(col_latch, 1);
        
        // reduces ghosting
        delayNanoseconds(250);
        clearDisplay();
        delayNanoseconds(50);
    }

    brightAt++;
    if (brightAt == brightnessLevels) {
        brightAt = 0;
    }
}

void LEDMatrix::clearDisplay() {
    digitalWriteFast(row_latch, 0);
    for (int d = 0; d < HEIGHT; d++) {
        digitalWriteFast(row_clock, 0);
        digitalWriteFast(row_data, 0);
        digitalWriteFast(row_clock, 1);
        delayNanoseconds(50);
    }
    digitalWriteFast(row_latch, 1);
}

LEDMatrix::~LEDMatrix() {
    free(buffer2);
}
