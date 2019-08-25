/*
 *  Interrupt and PWM utilities for 16 bit Timer1 on ATmega168/328
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 */

#define TIMER1_A_PIN   9
#define TIMER1_B_PIN   10
#define TIMER1_ICP_PIN 8
#define TIMER1_CLK_PIN 5

#define TIMER1_RESOLUTION 65536UL  // Timer1 is 16 bit

// Placing nearly all the code in this .h file allows the functions to be
// inlined by the compiler.  In the very common case with constant values
// the compiler will perform all calculations and simply write constants
// to the hardware registers (for example, setPeriod).


class TimerCounter
{
#ifdef __AVR_ATmega4809__
  public:
    //****************************
    //  Configuration
    //****************************
    void initialize(unsigned long microseconds=1000000) __attribute__((always_inline)) {
//  TCCR1B = _BV(WGM13);        // set mode as phase and frequency correct pwm, stop the timer
//  TCCR1A = 0;                 // clear control register A 
  setPeriod(microseconds);
    }
    void setPeriod(unsigned long microseconds) __attribute__((always_inline)) {
  const unsigned long cycles = (F_CPU / 2000000) * microseconds;
  if (cycles < TIMER1_RESOLUTION) {
    //clockSelectBits = _BV(CS10);
    clockSelectBits = 0;
    pwmPeriod = cycles;
  } else
  if (cycles < TIMER1_RESOLUTION * 8) {
    //clockSelectBits = _BV(CS11);
    clockSelectBits = 0;    
    pwmPeriod = cycles / 8;
  } else
  if (cycles < TIMER1_RESOLUTION * 64) {
    //clockSelectBits = _BV(CS11) | _BV(CS10);
    clockSelectBits = 0;    
    pwmPeriod = cycles / 64;
  } else
  if (cycles < TIMER1_RESOLUTION * 256) {
    //clockSelectBits = _BV(CS12);
    clockSelectBits = 0;   
    pwmPeriod = cycles / 256;
  } else
  if (cycles < TIMER1_RESOLUTION * 1024) {
    //clockSelectBits = _BV(CS12) | _BV(CS10);
    clockSelectBits = 0;    
    pwmPeriod = cycles / 1024;
  } else {
    //clockSelectBits = _BV(CS12) | _BV(CS10);
    clockSelectBits = 0;    
    pwmPeriod = TIMER1_RESOLUTION - 1;
  }
  //ICR1 = pwmPeriod;
  //TCCR1B = _BV(WGM13) | clockSelectBits;
    }

    //****************************
    //  Run Control
    //****************************
    void start() __attribute__((always_inline)) {
  //TCCR1B = 0;
  //TCNT1 = 0;    // TODO: does this cause an undesired interrupt?
  resume();
    }
    void stop() __attribute__((always_inline)) {
  //TCCR1B = _BV(WGM13);
    }
    void restart() __attribute__((always_inline)) {
  start();
    }
    void resume() __attribute__((always_inline)) {
  //TCCR1B = _BV(WGM13) | clockSelectBits;
    }

    //****************************
    //  Interrupt Function
    //****************************
    void attachInterrupt(void (*isr)()) __attribute__((always_inline)) {
  isrCallback = isr;
  //TIMSK1 = _BV(TOIE1);
    }
    void attachInterrupt(void (*isr)(), unsigned long microseconds) __attribute__((always_inline)) {
  if(microseconds > 0) setPeriod(microseconds);
  attachInterrupt(isr);
    }
    void detachInterrupt() __attribute__((always_inline)) {
  //TIMSK1 = 0;
    }
    static void (*isrCallback)();

  private:
    // properties
    static unsigned short pwmPeriod;
    static unsigned char clockSelectBits;
#else  //__AVR_ATmega328P__
  public:
    //****************************
    //  Configuration
    //****************************
    void initialize(unsigned long microseconds=1000000) __attribute__((always_inline)) {
  TCCR1B = _BV(WGM13);        // set mode as phase and frequency correct pwm, stop the timer
  TCCR1A = 0;                 // clear control register A 
  setPeriod(microseconds);
    }
    void setPeriod(unsigned long microseconds) __attribute__((always_inline)) {
  const unsigned long cycles = (F_CPU / 2000000) * microseconds;
  if (cycles < TIMER1_RESOLUTION) {
    clockSelectBits = _BV(CS10);
    pwmPeriod = cycles;
  } else
  if (cycles < TIMER1_RESOLUTION * 8) {
    clockSelectBits = _BV(CS11);
    pwmPeriod = cycles / 8;
  } else
  if (cycles < TIMER1_RESOLUTION * 64) {
    clockSelectBits = _BV(CS11) | _BV(CS10);
    pwmPeriod = cycles / 64;
  } else
  if (cycles < TIMER1_RESOLUTION * 256) {
    clockSelectBits = _BV(CS12);
    pwmPeriod = cycles / 256;
  } else
  if (cycles < TIMER1_RESOLUTION * 1024) {
    clockSelectBits = _BV(CS12) | _BV(CS10);
    pwmPeriod = cycles / 1024;
  } else {
    clockSelectBits = _BV(CS12) | _BV(CS10);
    pwmPeriod = TIMER1_RESOLUTION - 1;
  }
  ICR1 = pwmPeriod;
  TCCR1B = _BV(WGM13) | clockSelectBits;
    }

    //****************************
    //  Run Control
    //****************************
    void start() __attribute__((always_inline)) {
  TCCR1B = 0;
  TCNT1 = 0;    // TODO: does this cause an undesired interrupt?
  resume();
    }
    void stop() __attribute__((always_inline)) {
  TCCR1B = _BV(WGM13);
    }
    void restart() __attribute__((always_inline)) {
  start();
    }
    void resume() __attribute__((always_inline)) {
  TCCR1B = _BV(WGM13) | clockSelectBits;
    }

    //****************************
    //  Interrupt Function
    //****************************
    void attachInterrupt(void (*isr)()) __attribute__((always_inline)) {
  isrCallback = isr;
  TIMSK1 = _BV(TOIE1);
    }
    void attachInterrupt(void (*isr)(), unsigned long microseconds) __attribute__((always_inline)) {
  if(microseconds > 0) setPeriod(microseconds);
  attachInterrupt(isr);
    }
    void detachInterrupt() __attribute__((always_inline)) {
  TIMSK1 = 0;
    }
    static void (*isrCallback)();

  private:
    // properties
    static unsigned short pwmPeriod;
    static unsigned char clockSelectBits;
#endif

};

//extern TimerCounter Timer1;


