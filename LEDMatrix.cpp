#include "LEDMatrix.h"

#ifndef TEENSYDUINO
void delayNanoseconds(uint8_t c)
{
	for (uint8_t i = 0; i < c; i++){}
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS/DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////
LEDMatrix::LEDMatrix(uint16_t width, uint16_t height, SPIClass& spiClass,
					 uint8_t rowClockPin, uint8_t rowLatchPin, uint8_t rowDataPin, uint8_t colLatchPin,
					 uint8_t levels, uint32_t onDurationUS)
: GFXcanvas8(width, height)
, _spi(spiClass)
, _rowClock(rowClockPin)
, _rowLatch(rowLatchPin)
, _rowData(rowDataPin)
, _colLatch(colLatchPin)
, _brightnessLevels(levels)
, _onDuration(onDurationUS)
, _brightAt(0)
, _currentRow(0)
{
	_displayBuffer = (uint8_t*)malloc(width * height);
	memset(_displayBuffer, 0, width * height);

	_brightAt = 0;
	_currentRow = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
LEDMatrix::~LEDMatrix()
{
	free(_displayBuffer);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// METHODS
////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::begin(uint32_t clockSpeed)
{
	_spi.begin();
	_spi.beginTransaction(SPISettings(clockSpeed, MSBFIRST, SPI_MODE0));

	pinMode(_rowClock, OUTPUT);
	pinMode(_rowLatch, OUTPUT);
	pinMode(_rowData, OUTPUT);
	pinMode(_colLatch, OUTPUT);

	clearDisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t LEDMatrix::getBrightnessLevels()
{
	return _brightnessLevels;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::setBrightnessLevels(uint8_t levels)
{
	_brightnessLevels = levels;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t LEDMatrix::getOnDuration()
{
	return _brightnessLevels;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::setOnDuration(uint32_t onDurationUS)
{
	_onDuration = onDurationUS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::flip()
{
	// note, not actually a flip because buffer is private in GFXcanvas8
	noInterrupts();
	memcpy(_displayBuffer, getBuffer(), WIDTH * HEIGHT);
	interrupts();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::display()
{
	// print out displayBuffer
	
	uint8_t registers = (WIDTH >> 3);
	for (uint16_t whichRow = 0; whichRow < HEIGHT; whichRow++)
	{
		digitalWriteFast(_colLatch, LOW);

		// write out the data for that row
		uint8_t colByte;
		for (int i = registers - 1; i >= 0; i--)
		{
			colByte = 0x00;
			for (uint8_t j = 0; j < 8; j++)
			{
				if (_displayBuffer[(whichRow * WIDTH) + (i * 8) + j] > _brightAt)
				{
					bitSet(colByte, j);
				}
			}
			_spi.transfer(colByte);
		}

		digitalWriteFast(_rowLatch, LOW);
		selectRow(whichRow);

		// trigger latches to refresh screen
		digitalWriteFast(_colLatch, HIGH); 
		digitalWriteFast(_rowLatch, HIGH);   

		delayMicroseconds(_onDuration); // for brightness
		
		clearRows();
	}

	_brightAt++;
	if(_brightAt == _brightnessLevels)
		_brightAt = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::displayRow()
{
	digitalWriteFast(_rowLatch, LOW);
	selectRow(_currentRow);
	digitalWriteFast(_rowLatch, HIGH);

	uint8_t registers = (WIDTH >> 3);
	for(uint8_t levels = 0; levels < _brightnessLevels; levels++)
	{
		digitalWriteFast(_colLatch, LOW);

		// write out the data for that row
		uint8_t colByte;
		for(int i = registers - 1; i >= 0; i--)
		{
			colByte = 0x00;
			for(uint8_t j = 0; j < 8; j++)
			{
				if(_displayBuffer[(_currentRow * WIDTH) + (i * 8) + j] > levels)
				{
					bitSet(colByte, j);
				}
			}
			_spi.transfer(colByte);
		}

		// trigger latches to refresh screen
		digitalWriteFast(_colLatch, HIGH);

		delayMicroseconds(_onDuration); // for brightness
	}

	clearColumns();

	_currentRow++;
	if(_currentRow == HEIGHT)
		_currentRow = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::clearDisplay()
{
	clearColumns();
	clearRows();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::debugDisplay(Print& serialClass)
{
	uint8_t pixel;

	uint8_t range = _brightnessLevels / 10;

	uint8_t range2 = range + range;
	uint8_t range3 = range2 + range;
	uint8_t range4 = range3 + range;
	uint8_t range5 = range4 + range;
	uint8_t range6 = range5 + range;
	uint8_t range7 = range6 + range;
	uint8_t range8 = range7 + range;

	uint16_t row;
	for(uint16_t y = 0; y < HEIGHT; y++)
	{
		row = (y * WIDTH);
		for(uint16_t x = 0; x < WIDTH; x++)
		{
			pixel = _displayBuffer[row + x];

			//From: https://github.com/ebenpack/laboratory/blob/master/ASCII/ASCII-grayscale-values.txt
			if(pixel > range8)
				serialClass.print("@");
			else if(pixel > range7)
				serialClass.print("%");
			else if(pixel > range6)
				serialClass.print("#");
			else if(pixel > range5)
				serialClass.print("*");
			else if(pixel > range4)
				serialClass.print("+");
			else if(pixel > range3)
				serialClass.print("=");
			else if(pixel > range2)
				serialClass.print("-");
			else if(pixel > range)
				serialClass.print(":");
			else if(pixel > 0)
				serialClass.print(".");
			else
				serialClass.print(" ");
			serialClass.print(" ");
		}
		serialClass.println();
	}
	serialClass.println();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::selectRow(uint8_t rowIndex)
{
	digitalWriteFast(_rowData, LOW);
	for(int y = 0; y < HEIGHT - 1 - rowIndex; y++)
	{
		delayNanoseconds(DELAY_NS);
		digitalWriteFast(_rowClock, HIGH);
		delayNanoseconds(DELAY_NS);
		digitalWriteFast(_rowClock, LOW);
	}

	digitalWriteFast(_rowData, HIGH);
	delayNanoseconds(DELAY_NS);
	digitalWriteFast(_rowClock, HIGH);
	delayNanoseconds(DELAY_NS);
	digitalWriteFast(_rowClock, LOW);
	digitalWriteFast(_rowData, LOW);

	for(int y = 0; y < rowIndex; y++)
	{
		delayNanoseconds(DELAY_NS);
		digitalWriteFast(_rowClock, HIGH);
		delayNanoseconds(DELAY_NS);
		digitalWriteFast(_rowClock, LOW);
	}
	delayNanoseconds(DELAY_NS);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::clearColumns()
{
	// takes a little longer than clearing the 
	// darlington array but produces clearer image

	digitalWriteFast(_colLatch, LOW);

	uint8_t registers = (WIDTH >> 3);
	for(uint8_t i = 0; i < registers; i++)
	{
		_spi.transfer(0x00);
	}

	digitalWriteFast(_colLatch, HIGH);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDMatrix::clearRows()
{
	digitalWriteFast(_rowLatch, LOW);
	digitalWriteFast(_rowData, LOW);

	for(int y = 0; y < HEIGHT; y++)
	{
		delayNanoseconds(DELAY_NS);
		digitalWriteFast(_rowClock, HIGH);
		delayNanoseconds(DELAY_NS);
		digitalWriteFast(_rowClock, LOW);
	}

	digitalWriteFast(_rowLatch, HIGH);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////