#define VERSION "v1.14 UniDuino"
// ---------------------------------------------------------------------------------
// DO NOT USE CLASS-10 CARDS on this project - they're too fast to operate using SPI
// ---------------------------------------------------------------------------------
/*
 *                                    Casduino
 *                             Written and tested by
 *                          Andrew Beer & Duncan Edwards
 *                          
 *              Designed for MSX (and anything else that uses CAS files)
 *              Load Cas files onto an SD card, and play them directly 
 *              without converting to WAV first!
 *              
 *              Directory system allows multiple layers,  to return to root 
 *              directory ensure a file titles ROOT (no extension).  Selecting
 *              this file returns you to the root. 
 *              
 *              The code was adapted with the help of Cas2Wav which was written by:
 *              Vincent van Dam (vincentd@erg.verweg.com)
 *              
 *              
 *              ***************************************************************
 *              Menu System:
 *              
 *                Baud Rate:
 *                  1200 Baud
 *                  2400 Baud
 *                  2700 Baud                
 *                  3600 Baud (experimental, not all MSX machine support this)
 *                 
 *                Motor Control:
 *                  On
 *                  Off
 *   
 */

#include <SdFat.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include "UniDuino.h"
LiquidCrystal_I2C lcd(0x3f,16,2);  // set the LCD address to 0x27 or 0x3f for a 16 chars and 2 line display

SdFat sd;    //Initialise Sd card 

SdFile entry;  //SD card file

#define filenameLength    100

char fileName[filenameLength+1];  //Current filename
char sfileName[13];
char prevSubDir[3][25];
int subdir = 0;
unsigned long filesize;
const int chipSelect = 10;    //Sd card chip select pin

//#define oldCard       SPI_HALF_SPEED
//#define newCard       SPI_QUARTER_SPEED
//#define fastCard      SPI_FULL_SPEED

// ---------------------------------------------------------------------------------
// set cardType to 'oldCard' if using an old SD card (more than a few years old) or
// to 'newCard' if using a newly-purchase Class-4 card.
//int cardType = SPI_FULL_SPEED;
// ---------------------------------------------------------------------------------

#define btnPlay       17    //Play Button
#define btnStop       16    //Stop Button
#define btnUp         15    //Menu Up button
#define btnDown       14    //Menu Down button
#define btnMotor      6     //Motor Sense (connect pin to gnd to play, NC for pause)
#define btnMselect    7     //Motor Control on/off button 
#define scrollSpeed   250   //text scroll delay
#define scrollWait    3000  //Delay before scrolling starts
byte scrollPos=0;
unsigned long scrollTime=millis()+scrollWait;

byte mselectState = 1;//Motor control state 1=on 0=off
byte wasPlaying = 0;  //Was there a file playing?
byte motorState = 1;  //Current motor control state
byte oldMotorState = 1;
byte start = 0;       //Currently playing flag
byte pauseOn = 0;     //Pause state
int currentFile = 1; //Current position in directory
int maxFile = 0;     //Total number of files in directory
byte isDir = 0;       //Is the current file a directory
unsigned long timeDiff = 0;  //button debounce

byte UP = 0;                      //Next File, down button pressed

char PlayBytes[16];

void setup() {
  
  lcd.init();  //Initialise LCD (16x2 type)
  lcd.backlight();
  pinMode(chipSelect, OUTPUT); //Setup SD card chipselect pin
  if (!sd.begin(chipSelect,SPI_FULL_SPEED)) {  //Start SD card and check it's working
    lcd_clearline(0);
    lcd.print("No SD Card");
    return;
  }
  sd.chdir();  //set SD to root directory
  UniSetup();
  pinMode(btnPlay,INPUT_PULLUP);
  digitalWrite(btnPlay,HIGH);
  pinMode(btnStop,INPUT_PULLUP);
  digitalWrite(btnStop,HIGH);
  pinMode(btnUp,INPUT_PULLUP);
  digitalWrite(btnUp,HIGH);
  pinMode(btnDown,INPUT_PULLUP);
  digitalWrite(btnDown,HIGH);
  pinMode(btnMotor, INPUT_PULLUP);
  digitalWrite(btnMotor,HIGH);
  pinMode(btnMselect, INPUT_PULLUP);
  digitalWrite(btnMselect, HIGH);    //Setup buttons with internal pullup 
  
  //lcd.clear();
  //lcd.print(F(VERSION));
  lcd_clearline(0);  
  lcd.print(F("Reset.."));  
  delay(500);
  lcd.clear();
  
  getMaxFile();  //get the total number of files in the directory
  seekFile(currentFile);  //move to the first file in the directory
//  loadEEPROM();
  
  //lcd_clearline(0);
  //lcd.print(F(VERSION));
  
}

void loop(void) {
  if(start==1)
  {
    uniLoop();
  } else digitalWrite(outputPin, LOW);
  
  if((millis()>=scrollTime) && start==0 && (strlen(fileName)>15)) {
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
  if (millis() - timeDiff > 50) { // check switch every 100ms 
     timeDiff = millis(); // get current millisecond count
    
     if(digitalRead(btnPlay) == LOW) {
       //Handle Play/Pause button        
       if(start==0) {
         //If no file is play, start playback          
         playFile();
         delay(200);
       } else {
         //If a file is playing, pause or unpause the file              
         if (pauseOn == 0) {
           lcd_clearline(0);
           lcd.print(F("Paused"));
           pauseOn = 1;
         } else {
           lcd_clearline(0);
           lcd.print(PlayBytes);
           //lcd.print(F("Playing"));
           pauseOn = 0;
         }
         casPause();
       }
       while(digitalRead(btnPlay)==LOW) {
       //prevent button repeats by waiting until the button is released.
       delay(50);
       }     
     }

     if(digitalRead(btnMselect)==LOW && start==0){
       menuMode();
       setBaud();
       lcd_clearline(0);
       lcd.print(F(VERSION));
       lcd_clearline(1);
       scrollPos=0;
       scrollText(fileName);
       //sd.chdir();
       //getMaxFile();
       //currentFile=1;
       //seekFile(currentFile);         
       while(digitalRead(btnMselect)==LOW) {
         delay(50);
       }
     }
     if(digitalRead(btnStop)==LOW&& start==1) {
       stopFile();
       while(digitalRead(btnStop)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(50);
       }
     }
     
      if (digitalRead(btnStop)==LOW && start==0 && subdir >0){

       fileName[0]='\0';
       prevSubDir[subdir-1][0]='\0';
       subdir--;
       switch(subdir){
        case 1:
           //sprintf(fileName,"%s%s",prevSubDir[0],prevSubDir[1]);
           sd.chdir(strcat(strcat(fileName,"/"),prevSubDir[0]),true);
           break;
        case 2:
           //sprintf(fileName,"%s%s/%s",prevSubDir[0],prevSubDir[1],prevSubDir[2]);
           sd.chdir(strcat(strcat(strcat(strcat(fileName,"/"),prevSubDir[0]),"/"),prevSubDir[1]),true);
           break;
       case 3:
           //sprintf(fileName,"%s%s/%s/%s",prevSubDir[0],prevSubDir[1],prevSubDir[2],prevSubDir[3]);
          sd.chdir(strcat(strcat(strcat(strcat(strcat(strcat(fileName,"/"),prevSubDir[0]),"/"),prevSubDir[1]),"/"),prevSubDir[2]),true); 
           break;          
        default: 
           //sprintf(fileName,"%s",prevSubDir[0]);
           sd.chdir("/",true);
       }
       //Return to prev Dir of the SD card.
       //sd.chdir(fileName,true);
       //sd.chdir("/CDT");       
       //printtext(prevDir,0); //debug back dir
       getMaxFile();
       currentFile=1;
       seekFile(currentFile);  
       while(digitalRead(btnStop)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(50);
       }
     }     

     
     if(digitalRead(btnUp)==LOW && start==0) {
       scrollTime=millis()+scrollWait;
       scrollPos=0;
       upFile();       
       while(digitalRead(btnUp)==LOW) {
         delay(50);  //wait until button is released
       }
     }
     if(digitalRead(btnDown)==LOW && start==0) {
       scrollTime=millis()+scrollWait;
       scrollPos=0;
       downFile();
       while(digitalRead(btnDown)==LOW) {
         delay(50);
       }
     }
     if(start==1 && (!oldMotorState==motorState)) {  //if file is playing and motor control is on then handle current motor state
       if(motorState==1 && pauseOn==0) {         
         lcd_clearline(0);
         lcd.print(F("Paused"));         
         pauseOn = 1;
       } 
       if(motorState==0 && pauseOn==1) {
         casPause();
         lcd_clearline(0);
         lcd.print(F("Playing"));         
         pauseOn = 0;
       }
       casPause();
       oldMotorState=motorState;
     }   
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
  if(entry.isDir() || !strcmp(sfileName, "ROOT")) { isDir=1; } else { isDir=0; }
  entry.close();

  PlayBytes[0]='\0'; 
  if (isDir==1) {
    strcat_P(PlayBytes,PSTR(VERSION));
  } else {
    ltoa(filesize,PlayBytes,10);strcat_P(PlayBytes,PSTR(" bytes"));
  }

  lcd_clearline(0);
  lcd.print(PlayBytes);      
  
  lcd_clearline(1);
  scrollPos=0;
  scrollText(fileName);
}

void stopFile() {
  casStop();
  if(start==1){
    lcd_clearline(0);
    lcd.print(F("Stopped"));
    start=0;
  }
}

void playFile() {
  //PlayBytes[0]='\0';
  //strcat(PlayBytes,"Playing ");ltoa(filesize,PlayBytes+8,10);strcat(PlayBytes,"B");
  if(isDir==1) {
    //If selected file is a directory move into directory    
    changeDir();
  } else {
    if(entry.cwd()->exists(sfileName)) {
      lcd_clearline(0);
      lcd.print(F("Playing"));
      //lcd.print(PlayBytes);     
      lcd_clearline(1);
      scrollPos=0;
      scrollText(fileName);
      UniPlay(sfileName);
      wasPlaying = 1;
      start=1;       
      //lcd_clearline(0);
      //lcd.print(F("Paused"));
      //pauseOn = 1;
      //casPause();
    } else {
      lcd_clearline(1);
      lcd.print(F("No File Selected"));
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

void lcd_clearline(int l) {    
  //clear a single line on the LCD
  
  lcd.setCursor(0,l);
  lcd.print(F("                    "));
  lcd.setCursor(0,l);
}

void changeDir() {    
  //change directory, if fileName="ROOT" then return to the root directory
  //SDFat has no easy way to move up a directory, so returning to root is the easiest way. 
  //each directory (except the root) must have a file called ROOT (no extension)
                      
  if(!strcmp(fileName, "ROOT")) {
    subdir=0;   
    sd.chdir(true);
  } else {
     if (subdir >0) entry.cwd()->getName(prevSubDir[subdir-1],filenameLength); // Antes de cambiar  
     sd.chdir(fileName, true);
     subdir++;      
  }
  getMaxFile();
  currentFile=1;
  seekFile(currentFile);
}

void scrollText(char* text)
{
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
  lcd_clearline(1);
  lcd.print(outtext);
}
