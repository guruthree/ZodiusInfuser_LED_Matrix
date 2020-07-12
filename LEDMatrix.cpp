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

    // enable first row
	digitalWriteFast(row_data, 1);
	digitalWriteFast(row_clock, 0);
	delayNanoseconds(50);
	digitalWriteFast(row_clock, 1);
	delayNanoseconds(50);
	digitalWriteFast(row_data, 0);
	
    // for (int whichRow = 0; whichRow < HEIGHT; whichRow++) {
    for (int whichRow = HEIGHT - 1; whichRow >= 0; whichRow--) {

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

        // trigger latches to refresh screen
		digitalWriteFast(row_latch, 0);
        digitalWriteFast(col_latch, 0);
		delayNanoseconds(50);
        digitalWriteFast(row_latch, 1);
        digitalWriteFast(col_latch, 1);
		delayNanoseconds(50);
		delayNanoseconds(500); // for brightness
		
        // shift to next row
		digitalWriteFast(row_clock, 0);
		delayNanoseconds(50);
		digitalWriteFast(row_clock, 1);
    }

	clearDisplay();

    brightAt++;
    if (brightAt == brightnessLevels) {
        brightAt = 0;
    }
}

void LEDMatrix::clearDisplay() {
	// takes a little longer than clearing the 
	// darlington array but produces clearer image
	digitalWriteFast(col_latch, 0);
	for (int i = WIDTH/8 - 1; i >= 0; i--) {
		spi->transfer(0);
	}
	digitalWriteFast(col_latch, 1);
}

LEDMatrix::~LEDMatrix() {
    free(buffer2);
}
