#define VERSION "MaxDuino 1.41 M"
// ---------------------------------------------------------------------------------
// DO NOT USE CLASS-10 CARDS on this project - they're too fast to operate using SPI
// ---------------------------------------------------------------------------------
/*
 *                                    TZXduino
 *                             Written and tested by
 *                          Andrew Beer, Duncan Edwards
 *                          www.facebook.com/Arduitape/
 *                          
 *              Designed for TZX files for Spectrum (and more later)
 *              Load TZX files onto an SD card, and play them directly 
 *              without converting to WAV first!
 *              
 *              Directory system allows multiple layers,  to return to root 
 *              directory ensure a file titles ROOT (no extension) or by 
 *              pressing the Menu Select Button.
 *              
 *              Written using info from worldofspectrum.org
 *              and TZX2WAV code by Francisco Javier Crespo
 *              
 *              ***************************************************************
 *              Menu System:
 *                TODO: add ORIC and ATARI tap support, clean up code, sleep                 
 *                
 *              V1.0
 *                Motor Control Added.
 *                High compatibility with Spectrum TZX, and Tap files
 *                and CPC CDT and TZX files.
 *                
 *                V1.32 Added direct loading support of AY files using the SpecAY loader 
 *                to play Z80 coded files for the AY chip on any 128K or 48K with AY 
 *                expansion without the need to convert AY to TAP using FILE2TAP.EXE. 
 *                Download the AY loader from http://www.specay.co.uk/download 
 *                and load the LOADER.TAP AY file loader on your spectrum first then
 *                simply select any AY file and just hit play to load it. A complete
 *                set of extracted and DEMO AY files can be downloaded from
 *                http://www.worldofspectrum.org/projectay/index.htm
 *                Happy listening!
 *                
 *                V1.8.1 TSX support for MSX added by Natalia Pujol
 *                
 *                V1.8.2 Percentage counter and timer added by Rafael Molina Chesserot along with a reworking of the OLED1306 library. 
 *                Many memory usage improvements as well as a menu for TSX Baud Rates and a refined directory controls.
 *                
 *                V1.8.3 PCD8544 library changed to use less memory. Bitmaps added and Menu system reduced to a more basic level. 
 *                Bug fixes of the Percentage counter and timer when using motor control/
 *
 *                V1.8u1 (kernel@kernelcrash.com)  
 *                Mods to V1.8 TZXDuino to enable UEF playback. Only supports gunzip'd
 *                UEF files though (that still end in .uef). Plays back UEF's as square
 *                wave not sine waves.
 */ 
 //               MAXDUINO is a reworking from casduino and tzxduino to support both at the same time on Arduino Nano. 
 //               Original idea from Rafael Molina Chasserot, testing and help from Davide Barlotti, @ManuFerHi, @BCH, Alfredo Retrocant,
 //               and @Spirax who also provided modifications to support Oled double line with small font.
 //
 //               V1.28 Better speeed loading tzx files, more than 5000 baudios using PORT instructions
 //               V1.29 3-digits counter can be configured to display m:ss with #define CNTRBASE
 //               V1.30 max TSX speed 3850 vs  cas speed 3675. Also changed in Menu.
 //               V1.31 Modified ID20,2A to support automatic pausing(@spirax). Also swapped REW-FW for block navigation(@acf76es).
 //               V1.32 Deprecated old pskipPause and new block 2A pause control option in Menu.
 //               V1.33 Blocks for manual REW/FF expanded from 10 upto 20, used as circular buffer. On Oled screen prints upto 99 blocks,
 //                     overflowing to 00.  Selecting a block in pause mode traverse last 20 blocks.
 //               V1.34 alias "EEPROM version". Now blocks can be directly read/written from EEPROM.
 //                     Also logo can be copied to eeprom using #define option. After that, you can select option
 //                     for loading logo directly from EEPROM.. this saves 512 bytes in the sketch.
 //               V1.35 Uncompressed UEF support for Acorn computers, code imported from v1.8u1
 //               V1.35t Acorn speed for loading now 1500 baud, 20% faster (standard is 1200).
 //               V1.36 Polarity handling
 //               V1.37 Minor optimizations for counter format, acorn standard/turbo speed, simplication in polarity handling and 
 //                     now controlled from menu using the same option for tsx acceleration: tsxSPEEDzxPOL.
 //               V1.38 Code reformatted, cas is optional with new #def Use_CAS.Solved a bug when back with stop button from 4th subdir.
 //               V1.39 Optional hq.uef support for Acorn using define (implemented chunks 111 and 116).
 //                     Code better organized, and new userconfig.h with help for easier configuration -Alfredo Crespo
 //               V1.40 LCD sketch savings, tsx control/polarity/UEF turbo mode can be changed while paused with menu button. 
 //                     Reworked acorn chunk 116 floating point conversion to integer.
 //               V1.41 New logos from project Sugarless (@spark2k06/@yoxxxoy). New SPLASH_SCREEN configuration (@spark2k06).
 //                     Better .tsx start/stopbits support: Now working with SVI-3x8 machines!
 //
//
// =====================================================================================================================================
//
//          The "MaxDuino 1.41 M" version is based on the "v1.41 MaxDuino" version by @rcmolina.
//          Original idea of this "M" version by @merlinkv
//          My "M" version explained:
//            - The original @rcmolina version has been modified to be used on Arduino Mega 2560
//            - Added support to I2C LCD 20x4 screens
//            - Added support to I2C OLED 128x64 screens (using Adafruit_SH1106 library)
//            - Added an extra button ("Delete") to allow the deletion of single files
//            - On LCD 20x4 and OLED 128x64 screens it shows selected options Baud (Baud rate), Motor (Motor Ctrl), Blk2A (Skip Pauses) & TSXCo (TSX Control)
//          @rcmolina, @Spirax, @ManuFerHi, @BCH, @Pachuquin, @spark2k06 and all people involved ... Thanks for your work & help !!!
//
// =====================================================================================================================================
//
 
#include <SdFat.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include "userconfig.h"
#include "MaxDuino.h"

#ifdef LCDSCREEN16x2
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
  char indicators[] = {'|', '/', '-',0};
  uint8_t SpecialChar [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };
  #define SCREENSIZE 16
  #define maxblock 20
#endif

#ifdef LCDSCREEN20x4
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 20 chars and 4 line display
  char indicators[] = {'|', '/', '-',0};
  uint8_t SpecialChar [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };
  #define SCREENSIZE 20
  #define maxblock 10
#endif

#ifdef OLED1306 
  #include <Wire.h> 
  char indicators[] = {'|', '/', '-',92};
  #define SCREENSIZE 16
  #define maxblock 10
#endif

#ifdef OLEDSH1106
  #include <SPI.h> 
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SH1106.h>
  #define OLED_RESET 4
  Adafruit_SH1106 display(OLED_RESET);
  char indicators[] = {'|', '/', '-',92};
  #define SCREENSIZE 16
  #define maxblock 10
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
  #define maxblock 20
#endif

char fline[17];

SdFat sd;                           //Initialise Sd card 

SdFile entry;                       //SD card file

#define subdirLength     22         // hasta 62 no incrementa el consumo de RAM
#define filenameLength   4*subdirLength  //Maximum length for scrolling filename, hasta 255 no incrementa consumo RAM

char fileName[filenameLength + 1];    //Current filename
char sfileName[13];                   //Short filename variable
char prevSubDir[3][subdirLength];    // Subir a la EPROM ¿?
int DirFilePos[3];                   //File Positios in Directory to be restored
byte subdir = 0;
unsigned long filesize;             // filesize used for dimensioning AY files
const byte chipSelect = 53;          //Sd card chip select pin

int Delete = 0;

#define btnUp         A0            //Up button
#define btnDown       A1            //Down button
#define btnPlay       A2            //Play Button
#define btnStop       A3            //Stop Button
#define btnRoot       A4            //Return to SD card root
#define btnDelete     A5            //Delete Button
#define btnMotor      6             //Motor Sense (connect pin to gnd to play, NC for pause)
#define scrollSpeed   250           //text scroll delay
#define scrollWait    2500          //Delay before scrolling starts

byte scrollPos = 0;                 //Stores scrolling text position
unsigned long scrollTime = millis() + scrollWait;

// byte mselectMask = 0;              //Motor control state 1=on 0=off
byte motorState = 1;                //Current motor control state
byte oldMotorState = 1;             //Last motor control state
byte start = 0;                     //Currently playing flag
byte pskipMask = 0;                 // Pauses allowed
byte pauseOn = 0;                   //Pause state
int currentFile = 1;                //Current position in directory
int maxFile = 0;                    //Total number of files in directory
byte isDir = 0;                     //Is the current file a directory
unsigned long timeDiff = 0;         //button debounce

#if (SPLASH_SCREEN && TIMEOUT_RESET)
    unsigned long timeDiff_reset = 0;
    byte timeout_reset = TIMEOUT_RESET;
#endif

byte UP = 0;                      //Next File, down button pressed
char PlayBytes[subdirLength];

unsigned long blockOffset[maxblock];
byte blockID[maxblock];

#if (SPLASH_SCREEN && TIMEOUT_RESET)
    void(* resetFunc) (void) = 0;//declare reset function at adress 0
    /*void resetFunc() // Restarts program from beginning but does not reset the peripherals and registers
    {
    asm volatile ("  jmp 0");
    }*/
#endif

void setup() {
  
  #ifdef LCDSCREEN16x2
    lcd.init();                     //Initialise LCD (16x2 type)
    lcd.backlight();
    lcd.clear();
    #if (SPLASH_SCREEN)
        lcd.setCursor(0,0);
        lcd.print(F("Welcome to")); // Set the text at the initilization for LCD Screen (Line 1)
        lcd.setCursor(0,1); 
        lcd.print(F("Maxduino")); // Set the text at the initilization for LCD Screen (Line 2)
    #endif   
  #endif
  #ifdef LCDSCREEN20x4
    lcd.init();                     //Initialise LCD (20x4 type)
    lcd.backlight();
    lcd.clear();
    #if (SPLASH_SCREEN)
        lcd.setCursor(0,0);
        lcd.print(F("Welcome to")); // Set the text at the initilization for LCD Screen (Line 1)
        lcd.setCursor(0,1); 
        lcd.print(F("Maxduino")); // Set the text at the initilization for LCD Screen (Line 2)
    #endif   
  #endif
  #ifdef SERIALSCREEN
    Serial.begin(115200);
  #endif
  #ifdef OLED1306 
    Wire.begin();
    init_OLED();
    #if (!SPLASH_SCREEN)
        delay(1500);              // Show logo
        reset_display();           // Clear logo and load saved mode
    #endif
  #endif
  #ifdef OLEDSH1106
    delay(1000);
    Wire.begin();
    Serial.begin(9600);
    display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)    
    display.display();
    delay(2000);
    display.clearDisplay(); display.display();
    delay(1500);
  #endif
  #ifdef P8544 
    lcd.begin ();
    analogWrite (backlight_pin, 20);
    //lcd.clear();
    P8544_splash(); 
  #endif

//General Pin settings
 
// pinMode(btnUp,INPUT_PULLUP);  // Not needed, default is INPUT (0)
// digitalWrite(btnUp,HIGH);
// Analog pin A0
PORTF |= _BV(0);

// pinMode(btnDown,INPUT_PULLUP);  // Not needed, default is INPUT (0)
// digitalWrite(btnDown,HIGH);
// Analog pin A1
PORTF |= _BV(1);
  
// pinMode(btnPlay,INPUT_PULLUP);  // Not needed, default is INPUT (0)
// digitalWrite(btnPlay,HIGH);
// Analog pin A2
PORTF |= _BV(2);
  
// pinMode(btnStop,INPUT_PULLUP);  // Not needed, default is INPUT (0)
// digitalWrite(btnStop,HIGH);
// Analog pin A3
PORTF |= _BV(3);

// pinMode(btnRoot, INPUT_PULLUP);  // Not needed, default is INPUT (0)
// digitalWrite(btnRoot, HIGH); 
// Analog pin A4
PORTF |= _BV(4);

// pinMode(btnDelete, INPUT_PULLUP);  // Not needed, default is INPUT (0)
// digitalWrite(btnDelete,HIGH);
//Analog pin A5
PORTF |= _BV(5);

  #ifdef SPLASH_SCREEN
      while (digitalRead(btnPlay) == HIGH & 
             digitalRead(btnStop) == HIGH &
             digitalRead(btnUp)   == HIGH &
             digitalRead(btnDown) == HIGH &
             digitalRead(btnRoot) == HIGH)
      {
        delay(100);              // Show logo (OLED) or text (LCD) and remains until a button is pressed.
      }   
      #ifdef OLED1306    
          reset_display();           // Clear logo and load saved mode
      #endif
      #ifdef OLEDSH1106    
        Adafruit_SH1106 display(OLED_RESET);
        display.clearDisplay(); display.display();
        delay(500);
      #endif
  #endif
  
  pinMode(chipSelect, OUTPUT);      //Setup SD card chipselect pin
    while (!sd.begin(chipSelect,SPI_FULL_SPEED)) {      
    printtextF(PSTR("No SD Card"),0);
  } 
  sd.chdir();                       //set SD to root directory
  UniSetup();                       //Setup TZX specific options
  #ifdef OLEDSH1106
    display.setCursor(0,0);display.println("Ready ...");display.display();
    delay(1500);
  #else
    printtextF(PSTR("Ready..."),0);
    delay(1500);
  #endif
  #ifdef LCDSCREEN16x2
    lcd.clear();
    delay(1500);
  #endif
  #ifdef LCDSCREEN20x4
    lcd.clear();
    delay(1000);    
    loadEEPROM();
    LCDBStatusLine();
  #endif
  #ifdef P8544
    lcd.clear();
  #endif
  getMaxFile();                     //get the total number of files in the directory
  seekFile(currentFile);            //move to the first file in the directory
  #ifdef Use_MENU
    loadEEPROM();
  #endif    
  #ifdef OLED1306
    OledStatusLine();
  #endif
  #ifdef OLEDSH1106
    OledStatusLine();
  #endif
}

void loop(void) {
  
  if(start==1)
  {
    //TZXLoop only runs if a file is playing, and keeps the buffer full.
    uniLoop();
  } else {
  //  digitalWrite(outputPin, LOW);    //Keep output LOW while no file is playing.
    WRITE_LOW;    
  }
  
  if((millis()>=scrollTime) && start==0 && (strlen(fileName)> SCREENSIZE)) {
    //Filename scrolling only runs if no file is playing to prevent I2C writes 
    //conflicting with the playback Interrupt
    scrollTime = millis()+scrollSpeed;
    scrollText(fileName);
    scrollPos +=1;
    if(scrollPos>strlen(fileName)) {
      scrollPos=0;
      scrollTime=millis()+scrollWait;
      scrollText(fileName);
    }
  }
  motorState=digitalRead(btnMotor);
  #if (SPLASH_SCREEN && TIMEOUT_RESET)
      if (millis() - timeDiff_reset > 1000) //check timeout reset every second
      {
        timeDiff_reset = millis(); // get current millisecond count
        if (start==0)
        {
          timeout_reset--;
          if (timeout_reset==0)
          {
            timeout_reset = TIMEOUT_RESET;
            resetFunc();
          }
        }
        else
        {
          timeout_reset = TIMEOUT_RESET;
        }    
      }
  #endif
    
  if (millis() - timeDiff > 50) {   // check switch every 50ms 
     timeDiff = millis();           // get current millisecond count
      
     if(digitalRead(btnPlay) == LOW) {
        //Handle Play/Pause button
        if(start==0) {
          //If no file is play, start playback
          playFile();
          if (mselectMask == 1){  
            oldMotorState = !motorState;  //Start in pause if Motor Control is selected
          }
          delay(200);
          
        } else {
          if (pauseOn == 0) printtextF(PSTR("Paused"),0); 
          else printtextF(PSTR("Playing"),0);      
          #ifdef LCDSCREEN16x2            
            if (currpct <100) {              
              itoa(newpct,PlayBytes,10);strcat_P(PlayBytes,PSTR("%"));lcd.setCursor(8,0);lcd.print(PlayBytes);
            }
            strcpy(PlayBytes,"000");
            #ifndef cntrMSS
              if ((lcdsegs %1000) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %1000) <100)itoa(lcdsegs%1000,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%1000,PlayBytes,10);
            #endif
            #ifdef cntrMSS
              if ((lcdsegs %600) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %600) <60)itoa(lcdsegs%600,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%600 /60 *100 + lcdsegs%60,PlayBytes,10);
            #endif  
            lcd.setCursor(13,0);lcd.print(PlayBytes); 
          #endif
          #ifdef LCDSCREEN20x4            
            if (currpct <100) {              
              itoa(newpct,PlayBytes,10);strcat_P(PlayBytes,PSTR("%"));lcd.setCursor(8,0);lcd.print(PlayBytes);
            }
            strcpy(PlayBytes,"000");
            #ifndef cntrMSS
              if ((lcdsegs %1000) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %1000) <100)itoa(lcdsegs%1000,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%1000,PlayBytes,10);
            #endif
            #ifdef cntrMSS
              if ((lcdsegs %600) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %600) <60)itoa(lcdsegs%600,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%600 /60 *100 + lcdsegs%60,PlayBytes,10);
            #endif  
            lcd.setCursor(17,0);lcd.print(PlayBytes); 
          #endif          
          #ifdef OLED1306
            if (currpct <100) {                         
              itoa(newpct,PlayBytes,10);strcat_P(PlayBytes,PSTR("%"));setXY (8,0);sendStr(PlayBytes);
            }              
            strcpy(PlayBytes,"000");
            #ifndef cntrMSS
              if ((lcdsegs %1000) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %1000) <100)itoa(lcdsegs%1000,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%1000,PlayBytes,10);
            #endif
            #ifdef cntrMSS
              if ((lcdsegs %600) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %600) <60)itoa(lcdsegs%600,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%600 /60 *100 + lcdsegs%60,PlayBytes,10);
            #endif                 
            setXY(13,0);
            sendStr(PlayBytes);
          #endif
          #ifdef OLEDSH1106
            if (currpct <100) {                         
              itoa(newpct,PlayBytes,10);strcat_P(PlayBytes,PSTR("%"));setXY (8,0);sendStr(PlayBytes);
            }              
            strcpy(PlayBytes,"000");
            #ifndef cntrMSS
              if ((lcdsegs %1000) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %1000) <100)itoa(lcdsegs%1000,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%1000,PlayBytes,10);
            #endif
            #ifdef cntrMSS
              if ((lcdsegs %600) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %600) <60)itoa(lcdsegs%600,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%600 /60 *100 + lcdsegs%60,PlayBytes,10);
            #endif                 
            setXY(13,0);
            sendStr(PlayBytes);
          #endif
          #ifdef P8544                                      
            if (currpct <100) {              
              itoa(newpct,PlayBytes,10);strcat_P(PlayBytes,PSTR("%"));lcd.setCursor(0,3);lcd.print(PlayBytes);
            }
            strcpy(PlayBytes,"000");
            #ifndef cntrMSS
              if ((lcdsegs %1000) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %1000) <100)itoa(lcdsegs%1000,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%1000,PlayBytes,10);
            #endif
            #ifdef cntrMSS
              if ((lcdsegs %600) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %600) <60)itoa(lcdsegs%600,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%600 /60 *100 + lcdsegs%60,PlayBytes,10);
            #endif  
            lcd.gotoRc(3,38);
            lcd.bitmap(Paused, 1, 6);
          #endif
          pauseOn = !pauseOn;
       }
       debounce(btnPlay);         
     }
     if(digitalRead(btnRoot)==LOW && start==1 && pauseOn==1){
       // change tsx speed control/zx polarity/uefTurboMode
       TSXCONTROLzxpolarityUEFTURBO = !TSXCONTROLzxpolarityUEFTURBO;
       #ifdef OLED1306 
          OledStatusLine();
       #endif
       #ifdef OLEDSH1106 
          OledStatusLine();
       #endif         
       debounce(btnRoot);  
     }
     if(digitalRead(btnRoot)==LOW && start==0){
       #if (SPLASH_SCREEN && TIMEOUT_RESET)
            timeout_reset = TIMEOUT_RESET;
       #endif
        #ifdef Use_MENU
           menuMode();
           printtextF(PSTR(VERSION),0);
          #ifdef LCDSCREEN16x2
            printtextF(PSTR(""),1);
          #endif      
         #ifdef LCDSCREEN20x4
           printtextF(PSTR(""),1);
         #endif      
          #ifdef OLED1306
            printtextF(PSTR(""),lineaxy);
          #endif
         #ifdef OLEDSH1106
           printtextF(PSTR(""),lineaxy);
         #endif
         #ifdef P8544
            printtextF(PSTR(""),1);
          #endif      
          scrollPos=0;
          scrollText(fileName);
       #else             
         subdir=0;
         prevSubDir[0][0]='\0';
         prevSubDir[1][0]='\0';
         prevSubDir[2][0]='\0';
         sd.chdir(true);
         getMaxFile();
         currentFile=1;
         seekFile(currentFile);         
       #endif
       debounce(btnRoot);  
     }
     if(digitalRead(btnStop)==LOW && start==1) {
       stopFile();
       debounce(btnStop);
     }
     if(digitalRead(btnStop)==LOW && start==0 && subdir >0 && Delete==0) { 
       #if (SPLASH_SCREEN && TIMEOUT_RESET)
            timeout_reset = TIMEOUT_RESET;
       #endif     
       fileName[0]='\0';
       subdir--;
       prevSubDir[subdir][0]='\0';     
       switch(subdir){
        case 0:
           sd.chdir("/",true);
           break;
        case 1:
           sd.chdir(strcat(strcat(fileName,"/"),prevSubDir[0]),true);
           break;
        case 2:
        default:
           subdir = 2;
           sd.chdir(strcat(strcat(strcat(strcat(fileName,"/"),prevSubDir[0]),"/"),prevSubDir[1]),true);
           break;
       }
       getMaxFile();
       currentFile=DirFilePos[subdir];
       UP=1;
       seekFile(currentFile);
       DirFilePos[subdir]=0;
       debounce(btnStop);   
     }     
     if(digitalRead(btnUp)==LOW && start==1 && pauseOn==1 && mselectMask==1) {
       if (block > 0) block--;
       else block = 99; 
       EEPROM.get(BLOCK_EEPROM_START+5*block, bytesRead);
       EEPROM.get(BLOCK_EEPROM_START+4+5*block, currentID);
       currentTask=PROCESSID; 
       SetPlayBlock();       
       debounce(btnUp);         
     }
     if(digitalRead(btnUp)==LOW && start==0) {
       #if (SPLASH_SCREEN && TIMEOUT_RESET)
            timeout_reset = TIMEOUT_RESET;
       #endif
       scrollTime=millis()+scrollWait;
       scrollPos=0;
       upFile();
       debounce(btnUp);       
     }
     if(digitalRead(btnDown)==LOW && start==1 && pauseOn==1 && mselectMask==1) {
       if (block < 99) block++;
       else block = 0;
       EEPROM.get(BLOCK_EEPROM_START+5*block, bytesRead);
       EEPROM.get(BLOCK_EEPROM_START+4+5*block, currentID);
       currentTask=PROCESSID;
       SetPlayBlock(); 
       debounce(btnDown);                  
     }
     if(digitalRead(btnDown)==LOW && start==0 ) {
       #if (SPLASH_SCREEN && TIMEOUT_RESET)
            timeout_reset = TIMEOUT_RESET;
       #endif
       scrollTime=millis()+scrollWait;
       scrollPos=0;
       downFile();
       debounce(btnDown);      
     }
     if(start==1 && (!oldMotorState==motorState) && mselectMask==1 ) {  
       if(motorState==1 && pauseOn==0) {
         #ifdef LCDSCREEN16x2
              lcd.setCursor(0,0);
              lcd.print("PAUSED ");    
         #endif 
         #ifdef LCDSCREEN20x4
              lcd.setCursor(0,0);
              lcd.print("PAUSED ");    
         #endif
         #ifdef OLED1306
              sendStrXY("PAUSED ",0,0);
         #endif
         #ifdef OLEDSH1106
              sendStrXY("PAUSED ",0,0);
         #endif
         #ifdef P8544
              lcd.setCursor(0,0);
              lcd.print("PAUSED ");                       
         #endif                 
         scrollPos=0;
         scrollText(fileName);
         pauseOn = 1;
       } 
       if(motorState==0 && pauseOn==1) {
         #ifdef LCDSCREEN16x2
              lcd.setCursor(0,0);
              lcd.print("PLAYing");    
         #endif 
         #ifdef LCDSCREEN20x4
              lcd.setCursor(0,0);
              lcd.print("PLAYing");    
         #endif 
         #ifdef OLED1306
              sendStrXY("PLAYing",0,0);
         #endif
         #ifdef OLEDSH1106
              sendStrXY("PLAYing",0,0);
         #endif 
         #ifdef P8544
              lcd.setCursor(0,0);
              lcd.print("PLAYing");                       
         #endif            
         scrollPos=0;
         scrollText(fileName);
         pauseOn = 0;
       }
       oldMotorState=motorState;
     }
// Rutina de borrado de archivos

     if(digitalRead(btnDelete)==LOW && start==1 && Delete==0) {
       stopFile();
       debounce(btnDelete);
     }
     if(digitalRead(btnDelete)==LOW && start==0 && Delete==0) {
       #ifdef LCDSCREEN16x2
         lcd.setCursor(0,0);
         lcd.print("Delete File?    ");
       #endif
       #ifdef LCDSCREEN20x4
         lcd.setCursor(0,0);
         lcd.print("Delete File?        ");
         lcd.setCursor(0,2);
         lcd.print("Y Press Delete again");
         lcd.setCursor(0,3);
         lcd.print("N Press Stop        ");
       #endif
       #ifdef P8544
         lcd.setCursor(0,0);
         lcd.print("Delete File? ");
       #endif
       #ifdef OLED1306
         sendStrXY("Delete File? ",0,0);
       #endif
       #ifdef OLEDSH1106
         sendStrXY("Delete File?    ",0,3);
         sendStrXY("                ",0,4);
         sendStrXY("Y Press Dl again",0,5);
         sendStrXY("N Press Stop    ",0,6);
         sendStrXY("                ",0,7);         
       #endif
       Delete = 1;
       debounce(btnDelete);
     }
     if(digitalRead(btnStop)==LOW && start==0 && Delete==1) {
       #ifdef LCDSCREEN16x2
         lcd.clear();
       #endif
       #ifdef LCDSCREEN20x4
         lcd.clear();
         LCDBStatusLine();
       #endif
       #ifdef P8544
         lcd.clear();
       #endif
       #ifdef OLED1306
         clear_display();
         OledStatusLine();
       #endif
       #ifdef OLEDSH1106
         display.clearDisplay();display.display();
         OledStatusLine();         
       #endif
       printtext(PlayBytes,0);
       scrollPos=0;
       scrollText(fileName);       
       Delete = 0;
       debounce(btnStop);
     }
     if(digitalRead(btnDelete)==LOW && start==0 && Delete==1) {
       #ifdef LCDSCREEN16x2
         lcd.clear();
         lcd.print("File Deleted    ");
       #endif
       #ifdef LCDSCREEN20x4
         lcd.clear();
         lcd.print("File Deleted        ");
         LCDBStatusLine();
       #endif
       #ifdef P8544
         lcd.clear();
         lcd.print("File Deleted ");
       #endif
       #ifdef OLED1306
         clear_display();
         sendStrXY("File Deleted ",0,0);
       #endif         
       #ifdef OLEDSH1106
         display.clearDisplay();display.display();
         sendStrXY("File Deleted ",0,0);
         OledStatusLine();
       #endif
       sd.remove(fileName);
       delay(1000);
       downFile();
       seekFile(currentFile);
       delay(1000);
       scrollPos=0;
       scrollText(fileName);
       Delete = 0;
       debounce(btnDelete);
     }
// Fin de la rutina de borrado de archivos
  }
}

void debounce(int boton){
  while(digitalRead(boton)==LOW){
    //prevent button repeats by waiting until the button is released.
    delay(50);
  }
}

void upFile() {    
  //move up a file in the directory
  currentFile--;
  if(currentFile<1) {
    getMaxFile();
    currentFile = maxFile;
  }
  UP=1;   
  seekFile(currentFile);
}

void downFile() {    
  //move down a file in the directory
  currentFile++;
  if(currentFile>maxFile) { currentFile=1; }
  UP=0;  
  seekFile(currentFile);
}

void seekFile(int pos) {    
  //move to a set position in the directory, store the filename, and display the name on screen.
  if (UP==1) {  
    entry.cwd()->rewind();
    for(int i=1;i<=currentFile-1;i++) {
      entry.openNext(entry.cwd(),O_READ);
      entry.close();
    }
  }

  if (currentFile==1) {entry.cwd()->rewind();}
  entry.openNext(entry.cwd(),O_READ);
  entry.getName(fileName,filenameLength);
  entry.getSFN(sfileName);
  filesize = entry.fileSize();
  ayblklen = filesize + 3;  // add 3 file header, data byte and chksum byte to file length
  if(entry.isDir() || !strcmp(sfileName, "ROOT")) { isDir=1; } else { isDir=0; }
  entry.close();

  PlayBytes[0]='\0'; 
  if (isDir==1) {
 //   strcat_P(PlayBytes,PSTR(VERSION));
    //if (subdir >0)strncpy(PlayBytes,prevSubDir[subdir-1],16);
    if (subdir >0)strcpy(PlayBytes,prevSubDir[subdir-1]);
    else strcat_P(PlayBytes,PSTR(VERSION));
    #ifdef P8544
      printtext("                 ",3);
    #endif
    
  } else {
    ltoa(filesize,PlayBytes,10);strcat_P(PlayBytes,PSTR(" bytes"));
    #ifdef P8544
      printtext("                 ",3);
    #endif
  }

  //PlayBytes[0]='\0'; itoa(DirFilePos[0],PlayBytes,10); 
  printtext(PlayBytes,0);
  //printtext(prevSubDir[0],0);

  
  scrollPos=0;
  scrollText(fileName);
}

void stopFile() {
  //TZXStop();
  TZXStop();
  if(start==1){
    printtextF(PSTR("Stopped"),0);
    //lcd_clearline(0);
    //lcd.print(F("Stopped"));
    #ifdef P8544
      lcd.gotoRc(3,38);
      lcd.bitmap(Stop, 1, 6);
    #endif
    start=0;
  }
}

void playFile() {
  if(isDir==1) {
    changeDir();
  } else {
    if(entry.cwd()->exists(sfileName)) {
      printtextF(PSTR("Playing"),0);
      scrollPos=0;
      pauseOn = 0;
      scrollText(fileName);
      currpct=100;
      lcdsegs=0;
      UniPlay(sfileName);           //Load using the short filename
        #ifdef P8544
          lcd.gotoRc(3,38);
          lcd.bitmap(Play, 1, 6);
        #endif      
      start=1;       
    } else {
      #ifdef LCDSCREEN16x2
        printtextF(PSTR("No File Selected"),1);
      #endif
      #ifdef LCDSCREEN20x4
        printtextF(PSTR("No File Selected"),1);
      #endif      
      #ifdef OLED1306
        printtextF(PSTR("No File Selected"),lineaxy);
      #endif
      #ifdef OLEDSH1106
        printtextF(PSTR("No File Selected"),lineaxy);
      #endif
      #ifdef P8544
        printtextF(PSTR("No File Selected"),1);
      #endif
    }
  }
}

void getMaxFile() {    
  //gets the total files in the current directory and stores the number in maxFile
  
  entry.cwd()->rewind();
  maxFile=0;
  while(entry.openNext(entry.cwd(),O_READ)) {
    //entry.getName(fileName,filenameLength);
    entry.close();
    maxFile++;
  }
  entry.cwd()->rewind();
}



void changeDir() {    
  //change directory, if fileName="ROOT" then return to the root directory
  //SDFat has no easy way to move up a directory, so returning to root is the easiest way. 
  //each directory (except the root) must have a file called ROOT (no extension)
                      
  if(!strcmp(fileName, "ROOT")) {
    subdir=0;    
    sd.chdir(true);
  } else {
     //if (subdir >0) entry.cwd()->getName(prevSubDir[subdir-1],filenameLength); // Antes de cambiar
     DirFilePos[subdir] = currentFile;
     sd.chdir(fileName, true);
     if (strlen(fileName) > subdirLength) {
      //entry.getSFN(sfileName);
      strcpy(prevSubDir[subdir], sfileName);
     }
     else {
      strcpy(prevSubDir[subdir], fileName);
     }
     
     //entry.cwd()->getName(prevSubDir[subdir],filenameLength);
     //entry.getSFN(sfileName);
     //strcpy(prevSubDir[subdir], sfileName);
     //strcpy(prevSubDir[subdir], fileName);
     
     subdir++;      
  }
  getMaxFile();
  currentFile=1;
  seekFile(currentFile);
}

void scrollText(char* text)
{
  #ifdef LCDSCREEN16x2
  if(scrollPos<0) scrollPos=0;
  char outtext[17];
  if(isDir) { outtext[0]= 0x3E; 
    for(int i=1;i<16;i++)
    {
      int p=i+scrollPos-1;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  } else { 
    for(int i=0;i<16;i++)
    {
      int p=i+scrollPos;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  }
  outtext[16]='\0';
  printtext(outtext,1);
  #endif

  #ifdef LCDSCREEN20x4
    if(scrollPos<0) scrollPos=0;
    char outtext[21];
    if(isDir) { outtext[0]= 0x3E; 
    for(int i=1;i<20;i++){
      int p=i+scrollPos-1;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
    } else { 
      for(int i=0;i<20;i++)
      {
        int p=i+scrollPos;
        if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  }
  outtext[20]='\0';
  printtext(outtext,1);
  #endif

  #ifdef OLED1306
  //Text scrolling routine.  Setup for 16x2 screen so will only display 16 chars
  if(scrollPos<0) scrollPos=0;
  char outtext[17];
  if(isDir) { outtext[0]= 0x3E; 
    for(int i=1;i<16;i++)
    {
      int p=i+scrollPos-1;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  } else { 
    for(int i=0;i<16;i++)
    {
      int p=i+scrollPos;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  }
  outtext[16]='\0';
  printtext(outtext,lineaxy);
  //lcd_clearline(1);
  //lcd.print(outtext);
  #endif

  #ifdef OLEDSH1106
  if(scrollPos<0) scrollPos=0;
  char outtext[17];
  if(isDir) { outtext[0]= 0x3E; 
    for(int i=1;i<16;i++)
    {
      int p=i+scrollPos-1;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  } else { 
    for(int i=0;i<16;i++)
    {
      int p=i+scrollPos;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  }
  outtext[16]='\0';
  printtext(outtext,lineaxy);
  //lcd_clearline(1);
  //lcd.print(outtext);
  #endif

  #ifdef P8544
  //Text scrolling routine.  Setup for 16x2 screen so will only display 16 chars
  if(scrollPos<0) scrollPos=0;
  char outtext[15];
  if(isDir) { outtext[0]= 0x3E; 
    for(int i=1;i<14;i++)
    {
      int p=i+scrollPos-1;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  } else { 
    for(int i=0;i<14;i++)
    {
      int p=i+scrollPos;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  }
  outtext[14]='\0';
  printtext(outtext,1);
  //lcd_clearline(1);
  //lcd.print(outtext);
  #endif
}

/*
void lcd_clearline(int l) {    
  //clear a single line on the LCD
  
  lcd.setCursor(0,l);
  lcd.print(F("                    "));
  lcd.setCursor(0,l);
}
*/

void printtextF(const char* text, int l) {  //Print text to screen. 
  
  #ifdef SERIALSCREEN
  Serial.println(reinterpret_cast <const __FlashStringHelper *> (text));
  #endif
  
  #ifdef LCDSCREEN16x2
    strncpy_P(fline, text, 16);
    for(int i=strlen(fline);i<16;i++) fline[i]=0x20;
    lcd.setCursor(0,l);
    lcd.print(fline);
  #endif
  #ifdef LCDSCREEN20x4
    strncpy_P(fline, text, 20);
    for(int i=strlen(fline);i<20;i++) fline[i]=0x20;
    lcd.setCursor(0,l);
    lcd.print(fline);
  #endif
  #ifdef OLED1306
      strncpy_P(fline, text, 16);
      for(int i=strlen(fline);i<16;i++) fline[i]=0x20;
      sendStrXY(fline,0,l);
  #endif
  #ifdef OLEDSH1106
      strncpy_P(fline, text, 16);
      for(int i=strlen(fline);i<16;i++) fline[i]=0x20;
      sendStrXY(fline,0,l);
  #endif
  #ifdef P8544
    strncpy_P(fline, text, 14);
    for(int i=strlen(fline);i<14;i++) fline[i]=0x20;
    //lcd.setCursor(0,l);
    //lcd.print(F("              "));
    lcd.setCursor(0,l);
    lcd.print(fline);
    //lcd.print(reinterpret_cast <const __FlashStringHelper *> (text));
  #endif 
   
}

void printtext(char* text, int l) {  //Print text to screen. 
  #ifdef SERIALSCREEN
    Serial.println(text);
  #endif
  #ifdef LCDSCREEN16x2
    for(int i=0;i<16;i++)
    {
      if(i<strlen(text))  fline[i]=text[i];
      else  fline[i]=0x20;
    }   
    lcd.setCursor(0,l);
    lcd.print(fline);
  #endif
  #ifdef LCDSCREEN20x4
    for(int i=0;i<20;i++)
    {
      if(i<strlen(text))  fline[i]=text[i];
      else  fline[i]=0x20;
    }   
    lcd.setCursor(0,l);
    lcd.print(fline);
  #endif  
  #ifdef OLED1306
      for(int i=0;i<16;i++)
      {
        if(i<strlen(text))  fline[i]=text[i];
        else  fline[i]=0x20;
      }    
      sendStrXY(fline,0,l);
  #endif
  #ifdef OLEDSH1106
      for(int i=0;i<16;i++)
      {
        if(i<strlen(text))  fline[i]=text[i];
        else  fline[i]=0x20;
      }    
      sendStrXY(fline,0,l);
  #endif  
  #ifdef P8544
    for(int i=0;i<14;i++)
      {
        if(i<strlen(text))  fline[i]=text[i];
        else  fline[i]=0x20;
      }  
    //lcd.setCursor(0,l);
    //lcd.print(F("              "));
    lcd.setCursor(0,l);
    lcd.print(fline);
  #endif 
}

void OledStatusLine() {
  #ifdef OLED1306
    #ifdef XY
      setXY(0,2);
      sendStr("ID:   BLK:");
      setXY(0,3);
      sendChar(48+BAUDRATE/1000); sendChar(48+(BAUDRATE/100)%10);sendChar(48+(BAUDRATE/10)%10);sendChar(48+BAUDRATE%10);
      setXY(5,3);
      if(mselectMask==1) sendStr(" M:ON");
      else sendStr("m:off");    
        setXY(11,3); 
      if (skip2A == 1) sendStr(" S^ON");
      else sendStr("s^off");
    #endif
  #endif
  #ifdef OLEDSH1106
    #ifdef XY
      setXY(0,3);sendStr("BLK:    | ID:");
      setXY(0,4);sendStr("Baud --> ");
      setXY(0,5);sendStr("Motor -> ");
      setXY(0,6);sendStr("Blk2A -> ");
      setXY(0,7);sendStr("TSXCo -> ");      
      setXY(9,4);
      sendChar(48+BAUDRATE/1000); sendChar(48+(BAUDRATE/100)%10);sendChar(48+(BAUDRATE/10)%10);sendChar(48+BAUDRATE%10);
      setXY(9,5);
      if(mselectMask==1) sendStr("ON ");
      else sendStr("OFF");    
      setXY(9,6);
      if(pskipMask==1) sendStr("ON ");
      else sendStr("OFF");
      setXY(9,7);
      if (skip2A==1) sendStr("ON ");
      else sendStr("OFF");
    #endif
  #endif
}

void SetPlayBlock(){
  printtextF(PSTR(" "),0);
  #ifdef LCDSCREEN16x2
    lcd.setCursor(0,0);
    lcd.print("BLK:");
    lcd.print(block);lcd.print(' ');
    if (bytesRead > 0){
    lcd.print("ID:");lcd.print(currentID,HEX); // Block ID en hex
    }
  #endif
  #ifdef LCDSCREEN20x4
    lcd.setCursor(0,0);
    lcd.print("BLK:");
    lcd.print(block);lcd.print(' ');
    if (blockID[block] > 0){
      lcd.print("ID:");lcd.print(blockID[block],HEX); // Block ID en hex
    }
  #endif  
  #ifdef OLED1306
    setXY(0,0);
    sendStr("BLK:");
    utoa(block, input, 10);sendStr(input);sendChar(' ');
    if (bytesRead > 0){
      sendStr("ID:");utoa(currentID,input,16);sendStr(strupr(input)); // Block ID en hex
    }
  #endif
  #ifdef OLEDSH1106
    setXY(0,3);
    sendStr("BLK:");
    utoa(block, input, 10);sendStr(input);sendChar(' ');
    if (blockID[block] > 0){
      sendStr("ID:");utoa(blockID[block],input,16);sendStr(strupr(input)); // Block ID en hex
    }
  #endif
  #ifdef P8544
    lcd.setCursor(12,3);lcd.print('B'+block);
  #endif
  lcdsegs=0;       
  currentBit=0;                               // fallo reproducción de .tap tras .tzx
  pass=0;
  checkForEXT (sfileName);
  if (!casduino) {
    currentBlockTask = READPARAM;               //First block task is to read in parameters
    Timer1.setPeriod(1000);                     //set 1ms wait at start of a file.
  }
}

void LCDBStatusLine() {
  #ifdef LCDSCREEN20x4
    printtextF(PSTR("Baud:      Motor:   "),2);
    printtextF(PSTR("TSXC:      Skip2:   "),3);
    lcd.setCursor(5,2);
    lcd.print(BAUDRATE);
    lcd.setCursor(17,2);
    if(mselectMask==0)
      lcd.print("OFF");
    else
      lcd.print("ON");
    lcd.setCursor(6,3);
    if(TSXCONTROLzxpolarityUEFTURBO==0)
      lcd.print("OFF");
    else
      lcd.print("ON");
    lcd.setCursor(17,3);
    if(skip2A==0)
      lcd.print("OFF");
    else
      lcd.print("ON");
  #endif
}

