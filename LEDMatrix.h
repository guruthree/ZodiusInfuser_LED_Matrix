#ifndef _LEDMATRIX_H
#define _LEDMATRIX_H

/***** Includes *****/
#include <SPI.h>
#include <Adafruit_GFX.h>

/***** Defines *****/
#ifndef TEENSYDUINO
void delayNanoseconds(uint8_t c);
#define digitalWriteFast digitalWrite
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////////////////////////
class LEDMatrix : public GFXcanvas8
{
	//--------------------------------------------------
	// Constants
	//--------------------------------------------------
public:
	static const uint8_t MAX_BRIGHTNESS_LEVELS = 255;
	static const uint32_t DEFAULT_CLOCK_SPEED = 9000000;
	static const uint32_t DEFAULT_ON_DURATION = 1;
private:
	static const uint32_t DELAY_NS = 10;
	static const uint32_t BRIGHTNESS_US = 1;


	//--------------------------------------------------
	// Constructors/Destructor
	//--------------------------------------------------
public:
	LEDMatrix(uint16_t width, uint16_t height, SPIClass& spiClass, 
			  uint8_t rowClockPin, uint8_t rowLatchPin, uint8_t rowDataPin, uint8_t colLatchPin,
			  uint8_t levels = MAX_BRIGHTNESS_LEVELS, uint32_t onDurationUS = DEFAULT_ON_DURATION);
	virtual ~LEDMatrix(void); 


	//--------------------------------------------------
	// Methods
	//--------------------------------------------------
public:
	void begin(uint32_t clockSpeed = DEFAULT_CLOCK_SPEED);

	uint8_t getBrightnessLevels(void);
	void setBrightnessLevels(uint8_t levels);

	uint32_t getOnDuration(void);
	void setOnDuration(uint32_t onDurationUS);
	
	uint8_t wasLastFrameDrawn(void);

	uint8_t flip(void);
	void forceFlip(void);
	void display(void);
	void displayRow(void);
	void clearDisplay(void);

	void debugDisplay(Print& serialClass);

	//--------------------------------------------------
private:
	void selectRow(uint8_t rowIndex);
	void writeColumns(uint8_t whichRow, uint8_t levels);
	
	void clearColumns(void);
	void clearRows(void);


	//--------------------------------------------------
	// Variables
	//--------------------------------------------------
private:
	uint8_t* _displayBuffer;
	SPIClass& _spi;

	const uint8_t _rowClock;
	const uint8_t _rowLatch;
	const uint8_t _rowData;
	const uint8_t _colLatch;

	const uint8_t _registers;

	uint8_t _brightnessLevels;
	uint32_t _onDuration;

	uint8_t _brightAt;
	uint8_t _currentRow;
	uint8_t _lastFrameDrawn; // 1 when ready for next frame
};
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
