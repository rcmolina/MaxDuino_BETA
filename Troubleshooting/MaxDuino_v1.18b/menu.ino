/*
void menuMode()
 {
      //Return to root of the SD card.
       sd.chdir(true);
       getMaxFile();
       currentFile=1;
       seekFile(currentFile);  
       while(digitalRead(btnMselect)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(50);
       }
}
*/
/**************************************************************
 *                  Casduino Menu Code:
 *  Menu Button (was motor controll button) opens menu
 *  up/down move through menu, play select, stop back
 *  Menu Options:
 *  Baud:
 *    1200
 *    2400
 *    2700
 *    3600
 *  
 *  MotorControl:
 *    On
 *    Off
 *  
 *  Save settings to eeprom on exit. 
 */

 
 void menuMode()
 { 
  byte lastbtn=true;
  byte menuItem=0;
  byte subItem=0;
  byte updateScreen=true;
  
  while(digitalRead(btnStop)==HIGH || lastbtn)
  {
    if(updateScreen) {
      printtextF(PSTR("Menu Screen"),0);
      switch(menuItem) {
        case 0:
        printtextF(PSTR("Baud Rate"),1);
        break;
        case 1:
        printtextF(PSTR("Motor Control"),1);
        break;
        case 2:
        printtextF(PSTR("TSXspeedup ?"),1);
        break;       
      }
      updateScreen=false;
    }
    if(digitalRead(btnDown)==LOW && !lastbtn){
      if(menuItem<2) menuItem+=1;
      lastbtn=true;
      updateScreen=true;
    }
    if(digitalRead(btnUp)==LOW && !lastbtn) {
      if(menuItem>0) menuItem+=-1;
      lastbtn=true;
      updateScreen=true;
    }
    if(digitalRead(btnPlay)==LOW && !lastbtn) {
      switch(menuItem){
        case 0:
          subItem=0;
          updateScreen=true;
          lastbtn=true;
          while(digitalRead(btnStop)==HIGH || lastbtn) {
            if(updateScreen) {
              printtextF(PSTR("Baud Rate Menu"),0);
              switch(subItem) {
                case 0:                                  
                  printtextF(PSTR("1200 Baud       "),1);
                  if(BAUDRATE==1200) printtextF(PSTR("1200 Baud     * "),1);
                break;
                case 1:        
                  printtextF(PSTR("2400 Baud       "),1);
                  if(BAUDRATE==2400) printtextF(PSTR("2400 Baud     * "),1);
                break;
                case 2:                  
                  printtextF(PSTR("2700 Baud       "),1);
                  if(BAUDRATE==2700) printtextF(PSTR("2700 Baud     * "),1);
                break;                  
                case 3:                  
                  printtextF(PSTR("3600 Baud       "),1);
                  if(BAUDRATE==3600) printtextF(PSTR("3600 Baud     * "),1);
                break;                
              }
              updateScreen=false;
            }
                    
            if(digitalRead(btnDown)==LOW && !lastbtn){
              if(subItem<3) subItem+=1;
              lastbtn=true;
              updateScreen=true;
            }
            if(digitalRead(btnUp)==LOW && !lastbtn) {
              if(subItem>0) subItem+=-1;
              lastbtn=true;
              updateScreen=true;
            }
            if(digitalRead(btnPlay)==LOW && !lastbtn) {
              switch(subItem) {
                case 0:
                  BAUDRATE=1200;
                break;
                case 1:
                  BAUDRATE=2400;
                break;
                case 2:
                  BAUDRATE=2700;
                break;                
                case 3:
                  BAUDRATE=3600;
                break;
              }
              updateScreen=true;
              lastbtn=true;
            }
            if(digitalRead(btnDown) && digitalRead(btnUp) && digitalRead(btnPlay) && digitalRead(btnStop)) lastbtn=false;
          }
          lastbtn=true;
          updateScreen=true;
        break;
        case 1:
          subItem=0;
          updateScreen=true;
          lastbtn=true;
          while(digitalRead(btnStop)==HIGH || lastbtn) {
            if(updateScreen) {
              printtextF(PSTR("Motor Ctrl Menu "),0);
              switch(subItem) {
                case 0:
                  printtextF(PSTR("Motor Ctrl On   "),1);
                  if(mselectState==1) printtextF(PSTR("Motor Ctrl On * "),1);
                break;
                case 1:
                  printtextF(PSTR("Motor Ctrl Off  "),1);
                  if(mselectState==0) printtextF(PSTR("Motor Ctrl Off *"),1);
                break;                
              }
              updateScreen=false;
            }
                    
            if(digitalRead(btnDown)==LOW && !lastbtn){
              if(subItem<1) subItem+=1;
              lastbtn=true;
              updateScreen=true;
            }
            if(digitalRead(btnUp)==LOW && !lastbtn) {
              if(subItem>0) subItem+=-1;
              lastbtn=true;
              updateScreen=true;
            }
            if(digitalRead(btnPlay)==LOW && !lastbtn) {
              switch(subItem) {
                case 0:
                  mselectState=1;
                break;
                case 1:
                  mselectState=0;
                break;
              }
              lastbtn=true;
              updateScreen=true;
            }
            if(digitalRead(btnDown) && digitalRead(btnUp) && digitalRead(btnPlay) && digitalRead(btnStop)) lastbtn=false;
          }
          lastbtn=true;
          updateScreen=true;
        break;

        case 2:
          subItem=0;
          updateScreen=true;
          lastbtn=true;
          while(digitalRead(btnStop)==HIGH || lastbtn) {
            if(updateScreen) {
              printtextF(PSTR("TSXspeedup Menu "),0);
              switch(subItem) {
                case 0:
                  printtextF(PSTR("TSXspeedup Off  "),1);
                  if(TSXspeedup==0) printtextF(PSTR("TSXspeedup Off *"),1);
                break;
                case 1:
                  printtextF(PSTR("TSXspeedup On   "),1);
                  if(TSXspeedup==1) printtextF(PSTR("TSXspeedup On * "),1);
                break;                
              }
              updateScreen=false;
            }
                    
            if(digitalRead(btnDown)==LOW && !lastbtn){
              if(subItem<1) subItem+=1;
              lastbtn=true;
              updateScreen=true;
            }
            if(digitalRead(btnUp)==LOW && !lastbtn) {
              if(subItem>0) subItem+=-1;
              lastbtn=true;
              updateScreen=true;
            }
            if(digitalRead(btnPlay)==LOW && !lastbtn) {
              switch(subItem) {
                case 0:
                  TSXspeedup=0;
                break;
                case 1:
                  TSXspeedup=1;
                break;
              }
              lastbtn=true;
              updateScreen=true;
            }
            if(digitalRead(btnDown) && digitalRead(btnUp) && digitalRead(btnPlay) && digitalRead(btnStop)) lastbtn=false;
          }
          lastbtn=true;
          updateScreen=true;
        break;
        
      }
    }
    if(digitalRead(btnDown) && digitalRead(btnUp) && digitalRead(btnPlay) && digitalRead(btnStop)) lastbtn=false;
  }
  updateEEPROM();
 }

 void updateEEPROM()
 {
  /* Setting Byte: 
   *  bit 0: 1200
   *  bit 1: 2400
   *  bit 2: 2700
   *  bit 3: 3600
   *  bit 4: n/a
   *  bit 5: n/a
   *  bit 6: TSXspeedup
   *  bit 7: motor Control
   */
  byte settings=0;

  switch(BAUDRATE) {
    case 1200:
    settings |=1;
    break;
    case 2400:
    settings |=2;
    break;
    case 2700:
    settings |=4;  
    break;      
    case 3600:
    settings |=8;
    break;
  }
  if(mselectState) settings |=128;
  if(TSXspeedup) settings |=64; 
  EEPROM.put(0,settings);
 }

 void loadEEPROM()
 {
  byte settings=0;
  EEPROM.get(0,settings);
  if(!settings) return;
  
  if(bitRead(settings,7)) {
    mselectState=1;
  } else {
    mselectState=0;
  }
  if(bitRead(settings,6)) {
    TSXspeedup=1;
  } else {
    TSXspeedup=0;
  }
  
  if(bitRead(settings,0)) {
    BAUDRATE=1200;
  }
  if(bitRead(settings,1)) {
    BAUDRATE=2400;
  }
  if(bitRead(settings,2)) {
    BAUDRATE=2700;  
  }
  if(bitRead(settings,3)) {
    BAUDRATE=3600;  
  }
  setBaud();
  UniSetup();
 
 }

