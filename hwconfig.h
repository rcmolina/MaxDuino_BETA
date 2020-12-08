#ifdef __AVR_ATmega2560__
  #define outputPin           23 
  #define INIT_OUTPORT        DDRA |=  _BV(1)         // El pin23 es el bit1 del PORTA
  #define WRITE_LOW           PORTA &= ~_BV(1)         // El pin23 es el bit1 del PORTA
  #define WRITE_HIGH          PORTA |=  _BV(1)         // El pin23 es el bit1 del PORTA

#elif defined(__AVR_ATmega4809__)
  #define outputPin           9
  //#define INIT_OUTPORT         DDRB |=  _BV(1)         // El pin9 es el bit1 del PORTB
  //#define INIT_OUTPORT          pinMode(outputPin,OUTPUT)  
  #define INIT_OUTPORT         VPORTB.DIR |=  _BV(0)         // El pin9 es PB0
  //#define WRITE_LOW           PORTB &= ~_BV(1)         // El pin9 es el bit1 del PORTB
  //#define WRITE_LOW             digitalWrite(outputPin,LOW)
  #define WRITE_LOW           VPORTB.OUT &= ~_BV(0)         // El pin9 es PB0
  //#define WRITE_HIGH          PORTB |=  _BV(1)         // El pin9 es el bit1 del PORTB
  //#define WRITE_HIGH            digitalWrite(outputPin,HIGH)
  #define WRITE_HIGH          VPORTB.OUT |=  _BV(0)         // El pin9 es PB0

#elif defined(__AVR_ATmega4808__)
  #define outputPin           9
  //#define INIT_OUTPORT          pinMode(outputPin,OUTPUT)  
  #define INIT_OUTPORT         VPORTA.DIR |=  _BV(7)         // El pin9 es PA7
  //#define WRITE_LOW             digitalWrite(outputPin,LOW)
  #define WRITE_LOW           VPORTA.OUT &= ~_BV(7)         // El pin9 es PA7
  //#define WRITE_HIGH            digitalWrite(outputPin,HIGH)
  #define WRITE_HIGH          VPORTA.OUT |=  _BV(7)         // El pin9 es PA7

#elif defined(__arm__) && defined(__STM32F1__)
  #define outputPin     PA9    // this pin is 5V tolerant and PWM output capable 
  #define INIT_OUTPORT          pinMode(outputPin,OUTPUT)  
  #define WRITE_LOW             digitalWrite(outputPin,LOW)
  #define WRITE_HIGH            digitalWrite(outputPin,HIGH)
      
#else  //__AVR_ATmega328P__
  //#define MINIDUINO_AMPLI     // For A.Villena's Miniduino new design
  #define outputPin           9
  #ifdef MINIDUINO_AMPLI
    #define INIT_OUTPORT         DDRB |= B00000011                              // pin8+ pin9 es el bit0-bit1 del PORTB 
    #define WRITE_LOW           (PORTB &= B11111101) |= B00000001               // pin8+ pin9 , bit0- bit1 del PORTB
    #define WRITE_HIGH          (PORTB |= B00000010) &= B11111110               // pin8+ pin9 , bit0- bit1 del PORTB  
  //  #define WRITE_LOW           PORTB = (PORTB & B11111101) | B00000001         // pin8+ pin9 , bit0- bit1 del PORTB
  //  #define WRITE_HIGH          PORTB = (PORTB | B00000010) & B11111110         // pin8+ pin9 , bit0- bit1 del PORTB 
  #else
    #define INIT_OUTPORT         DDRB |=  _BV(1)         // El pin9 es el bit1 del PORTB
    #define WRITE_LOW           PORTB &= ~_BV(1)         // El pin9 es el bit1 del PORTB
    #define WRITE_HIGH          PORTB |=  _BV(1)         // El pin9 es el bit1 del PORTB
  #endif

// pin 0-7 PortD0-7, pin 8-13 PortB0-5, pin 14-19 PortC0-5

/*
#ifdef rpolarity 
  #define WRITE_LOW           PORTB &= ~_BV(1)        // El pin9 es el bit1 del PORTB
  #define WRITE_HIGH          PORTB |= _BV(1)         // El pin9 es el bit1 del PORTB
  // pin 0-7 PortD0-7, pin 8-13 PortB0-5, pin 14-19 PortC0-5
#endif

#ifndef rpolarity 
  #define WRITE_HIGH           PORTB &= ~_BV(1)        // El pin9 es el bit1 del PORTB
  #define WRITE_LOW          PORTB |= _BV(1)         // El pin9 es el bit1 del PORTB
  // pin 0-7 PortD0-7, pin 8-13 PortB0-5, pin 14-19 PortC0-5
#endif
*/

#endif 

#if defined(__AVR_ATmega4809__) || defined (__AVR_ATmega4808__)
                          //
                          // PATCH PROCERURE NEEDED FOR NANO EVERY AND THINARY TO SWAP TIMERS AND AVOID HANGING
                          // In C:\Users\Rafael\AppData\Local\Arduino15\packages\arduino\hardware\megaavr\1.8.4
                          // change .\variants\nona4809\timers.h , .\variants\nona4809\variant.c , .\cores\arduino\wiring.c
                          //
                          // In C:\Users\Rafael\AppData\Local\Arduino15\packages\thinary\hardware\avr\1.0.0
                          // change .\variants\nona4808\timers.h , .\variants\nona4808\variant.c , .\cores\arduino\wiring.c
                          //
                          // Needs 2 patches, check your version:
                          //
                          // SDFat 20150201 (old and very slow):
                          // 1. In SdFatConfig.h change line 84 #define SD_SPI_CONFIGURATION 0
                          //    with: #define SD_SPI_CONFIGURATION 1
                          // 2. In SdSpi.h change line 292 #ifdef __AVR__
                          //    with: #if defined(__AVR__) && not defined(__AVR_ATmega4809__)&& not defined(__AVR_ATmega4808__)
                          //
                          // SDFat 1.1.0 (faster and recommended):
                          // 1. In SdFatConfig.h change line 216 #elif defined(__AVR__)\
                          //    with: #elif defined(__AVR__) && not defined(__AVR_ATmega4809__)&& not defined(__AVR_ATmega4808__)\
                          //
                          // 2. In SpiDriver/SdSpiDriver.h change line 374 #ifdef __AVR__
                          //    with: #if defined(__AVR__) && not defined(__AVR_ATmega4809__)&& not defined(__AVR_ATmega4808__)
                         
  #define SDFat 
  //#define TimerOne 
#elif defined(__arm__) && defined(__STM32F1__)
  #define SDFat
  //#define TimerOne  
#else  //__AVR_ATmega328P__
  #define SDFat
  //#define TimerOne
#endif

#ifdef TimerOne
  #include <TimerOne.h>
#elif defined(__arm__) && defined(__STM32F1__)
  HardwareTimer timer(2); // channer 2  
  #include "itoa.h"  
  #define strncpy_P(a, b, n) strncpy((a), (b), (n))
  #define memcmp_P(a, b, n) memcmp((a), (b), (n))  
#else
  #include "TimerCounter.h"
  TimerCounter Timer1;              // preinstatiate
  
  unsigned short TimerCounter::pwmPeriod = 0;
  unsigned char TimerCounter::clockSelectBits = 0;
  void (*TimerCounter::isrCallback)() = NULL;
  
  // interrupt service routine that wraps a user defined function supplied by attachInterrupt
  #if defined(__AVR_ATmega4809__) || defined (__AVR_ATmega4808__)
    ISR(TCA0_OVF_vect)
    {
      Timer1.isrCallback();
    /* The interrupt flag has to be cleared manually */
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
    }  
  #else //__AVR_ATmega328P__
    ISR(TIMER1_OVF_vect)
    {
      Timer1.isrCallback();
    }
  #endif
#endif

#ifdef SDFat
  #include <SdFat.h>
#else
  #include <SD.h>
  #define SdFile File
  #define SdFat SDClass
  #define chdir open
  #define openNext openNextFile
  #define isDir() isDirectory()
  #define fileSize size
  #define seekSet seek
  File cwdentry;
#endif

#ifdef __AVR_ATmega2560__

  const byte chipSelect = 53;          //Sd card chip select pin
  
  #define btnUp         A0            //Up button
  #define btnDown       A1            //Down button
  #define btnPlay       A2            //Play Button
  #define btnStop       A3            //Stop Button
  #define btnRoot       A4            //Return to SD card root
  // #define btnDelete     A5         //Not implemented this button is for an optional function
  #define btnMotor      6             //Motor Sense (connect pin to gnd to play, NC for pause)

#elif defined(__arm__) && defined(__STM32F1__)
//
// Pin definition for Blue Pill boards
//

#define chipSelect    PB12            //Sd card chip select pin

#define btnPlay       PA0           //Play Button
#define btnStop       PA1           //Stop Button
#define btnUp         PA2           //Up button
#define btnDown       PA3           //Down button
#define btnMotor      PA8     //Motor Sense (connect pin to gnd to play, NC for pause)
#define btnRoot       PA4           //Return to SD card root

#else
  const byte chipSelect = 10;          //Sd card chip select pin
  
  #define btnPlay       17            //Play Button
  #define btnStop       16            //Stop Button
  #define btnUp         15            //Up button
  #define btnDown       14            //Down button
  #define btnMotor      6             //Motor Sense (connect pin to gnd to play, NC for pause)
  #define btnRoot       7             //Return to SD card root
#endif


#define scrollSpeed   250           //text scroll delay
#define scrollWait    3000          //Delay before scrolling starts

#ifdef LCDSCREEN16x2
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(LCD_I2C_ADDR,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
  char indicators[] = {'|', '/', '-',0};
  uint8_t SpecialChar [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };
  #define SCREENSIZE 16
#endif

#ifdef OLED1306 
  #include <Wire.h> 
  //#include "U8glib.h"
  //U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI 

  //char line0[17];
  //char line1[17];
  char indicators[] = {'|', '/', '-',92};
  #define SCREENSIZE 16
#endif

#ifdef P8544
  #include <pcd8544.h>
  #define ADMAX 1023
  #define ADPIN 0
  #include <avr/pgmspace.h>
  byte dc_pin = 5;
  byte reset_pin = 3;
  byte cs_pin = 4;
  pcd8544 lcd(dc_pin, reset_pin, cs_pin);
  char indicators[] = {'|', '/', '-',0};
  uint8_t SpecialChar [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };
  #define backlight_pin 2
  
  const byte Play [] PROGMEM = {
    0x00, 0x7f, 0x3e, 0x1c, 0x08, 0x00, 0x00
  };

  const byte Paused [] PROGMEM = {
    0x00, 0x7f, 0x7f, 0x00, 0x7f, 0x7f, 0x00
  };

  const byte Stop [] PROGMEM = {
    0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e
  };
  #define SCREENSIZE 14
#endif
