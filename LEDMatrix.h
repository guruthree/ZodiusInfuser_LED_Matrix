#ifndef _LEDMATRIX_H
#define _LEDMATRIX_H

#include <SPI.h>
#include <Adafruit_GFX.h>

#ifndef TEENSYDUINO
void delayNanoseconds(uint8_t c);
#define digitalWriteFast digitalWrite
#endif

#define MAX_BRIGHTNESS_LEVELS 255

class LEDMatrix : public GFXcanvas8 {

    public:
        LEDMatrix(uint16_t w, uint16_t h, SPIClass *spiClass, 
            uint8_t rowClock, uint8_t rowLatch, uint8_t rowData, uint8_t colLatch)
            : LEDMatrix(w, h, spiClass, rowClock, rowLatch, rowData, colLatch, MAX_BRIGHTNESS_LEVELS) {}
        LEDMatrix(uint16_t w, uint16_t h, SPIClass *spiClass, 
            uint8_t rowClock, uint8_t rowLatch, uint8_t rowData, uint8_t colLatch, uint8_t bl);
        void begin();
        void flip();
        void display();
        void clearDisplay();
        ~LEDMatrix(void);

        uint8_t getBrightnessLevels() { return brightnessLevels; }
        void setBrightnessLevels(uint8_t bl) {brightnessLevels = bl; }

    private:
        uint8_t* buffer2;
        SPIClass *spi;

        uint8_t row_clock;
        uint8_t row_latch;
        uint8_t row_data;
        uint8_t col_latch;

        uint8_t brightAt;
        uint8_t brightnessLevels;
};

#endif
