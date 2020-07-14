#include <SPI.h>
#include <Adafruit_GFX.h>
#include <LEDMatrix.h>

static const uint8_t ROW_CLOCK_PIN = 19;
static const uint8_t ROW_LATCH_PIN = 18;
static const uint8_t ROW_DATA_PIN = 17;
static const uint8_t COL_LATCH_PIN = 16;

// note ROW and COL are labelled backwards on the PCB
LEDMatrix gfx(32, 8, SPI, ROW_CLOCK_PIN, ROW_LATCH_PIN, ROW_DATA_PIN, COL_LATCH_PIN, LEDMatrix::MAX_BRIGHTNESS_LEVELS / 4, 4);

#ifdef TEENSYDUINO
IntervalTimer myTimer;
#else
#include <TimerOne.h>
#endif

int textx = gfx.width() + 1;
unsigned long currentMillis, textMillis, previousTextMillis = 0;

void myDisplay() // easiest way around member function pointer
{
  gfx.display();
// gfx.displayRow();
}

void setup()
{
  gfx.begin(20000000);
  gfx.setTextWrap(false);
  
  #ifdef TEENSYDUINO
    myTimer.begin(myDisplay, 70); // function takes ~50 microseconds
  #else
    Timer1.initialize(70);
    Timer1.attachInterrupt(myDisplay);
  #endif
}

void loop()
{
  // put your main code here, to run repeatedly:

  textMillis = currentMillis = millis();
  
  if((textMillis - previousTextMillis) >= 100)
  {    
    gfx.fillScreen(0);
//    gfx.drawPixel(0, 0, 1);
    gfx.drawLine(0, 0, 32, 0, 1);
    gfx.drawLine(0, 1, 32, 1, 2);
    gfx.drawLine(0, 2, 32, 2, 3);
    gfx.drawLine(0, 3, 32, 3, 4);
    gfx.drawLine(0, 4, 32, 4, 5);
    gfx.drawLine(0, 5, 32, 5, 6);
    gfx.drawLine(0, 6, 32, 6, 7);
    gfx.drawLine(0, 7, 32, 7, 8);
    gfx.setCursor(textx, 0);
    
//    gfx.print(todisplay);

    gfx.setTextColor(0);
    gfx.print("Hello ");
    gfx.setTextColor(gfx.getBrightnessLevels());
    gfx.print("Teensy!");

    previousTextMillis = textMillis;
//    if (textx == NUM_ACROSS - todisplay.length()*8 - 1)
    if(textx == gfx.width() - 13*8 - 1)
	{
      textx = gfx.width()+1;
    }
    else
	{
      textx--;
    }

    gfx.flip();
  }
}
