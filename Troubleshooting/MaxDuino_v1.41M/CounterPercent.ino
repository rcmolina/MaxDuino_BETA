
void lcdSpinner() {
    if (millis() - timeDiff2 > 1000) {   // check switch every 50ms 
        timeDiff2 = millis();           // get current millisecond count
        #ifdef LCDSCREEN16x2          
          lcd.setCursor(15,0);
          lcd.print(indicators[spinpos++]);
        #endif
        #ifdef LCDSCREEN20x4          
          lcd.setCursor(19,0);
          lcd.print(indicators[spinpos++]);
        #endif
        #ifdef OLED1306
            //sendCharXY(indicators[spinpos++],15,0);
           setXY(15,0);sendChar(indicators[spinpos++]);                 
        #endif
        #ifdef OLEDSH1106
            //sendCharXY(indicators[spinpos++],15,0);
           setXY(15,0);sendChar(indicators[spinpos++]);                 
        #endif
        #ifdef P8544
          lcd.setCursor(8,3);
          lcd.print(indicators[spinpos++]);
        #endif                
        if (spinpos > 3) {   
          spinpos = 0;
        }    
    } 
}

void lcdTime() {
    if (millis() - timeDiff2 > 1000) {   // check switch every second 
        timeDiff2 = millis();           // get current millisecond count
        #ifdef LCDSCREEN16x2
            if (lcdsegs % 10 != 0) {itoa(lcdsegs%10,PlayBytes,10);lcd.setCursor(15,0);lcd.print(PlayBytes);} // ultima cifra 1,2,3,4,5,6,7,8,9
            else 
               if (lcdsegs % CNTRBASE != 0){itoa(lcdsegs%CNTRBASE,PlayBytes,10);lcd.setCursor(14,0);lcd.print(PlayBytes);} // es 10,20,30,40,50,60,70,80,90,110,120,..
               else 
                  if (lcdsegs % (CNTRBASE*10) != 0) {itoa(lcdsegs%(CNTRBASE*10)/CNTRBASE*100,PlayBytes,10);lcd.setCursor(13,0);lcd.print(PlayBytes);} // es 100,200,300,400,500,600,700,800,900,1100,..
                  else {lcd.setCursor(13,0);lcd.print("000");} // es 000,1000,2000,...
           lcdsegs++;
        #endif
        #ifdef LCDSCREEN20x4
          if (lcdsegs % 10 != 0) {itoa(lcdsegs%10,PlayBytes,10);lcd.setCursor(19,0);lcd.print(PlayBytes);} // ultima cifra 1,2,3,4,5,6,7,8,9
          else
            if (lcdsegs % CNTRBASE != 0){itoa(lcdsegs%CNTRBASE,PlayBytes,10);lcd.setCursor(18,0);lcd.print(PlayBytes);} // es 10,20,30,40,50,60,70,80,90,110,120,..
            else
              if (lcdsegs % (CNTRBASE*10) != 0) {itoa(lcdsegs%(CNTRBASE*10) /CNTRBASE*100,PlayBytes,10);lcd.setCursor(17,0);lcd.print(PlayBytes);} // es 100,200,300,400,500,600,700,800,900,1100,..
              else {lcd.setCursor(17,0);lcd.print("000");} // es 000,1000,2000,...
          lcdsegs++;
        #endif
        #ifdef OLEDSH1106
            if (lcdsegs % 10 != 0) {setXY(15,0);sendChar(48+lcdsegs%10);} // ultima cifra 1,2,3,4,5,6,7,8,9
            else 
               if (lcdsegs % CNTRBASE != 0){setXY(14,0);sendChar(48+(lcdsegs%CNTRBASE)/10);sendChar('0');} // es 10,20,30,40,50,60,70,80,90,110,120,..
               else 
                  if (lcdsegs % (CNTRBASE*10) != 0) {setXY(13,0);sendChar(48+(lcdsegs% (CNTRBASE*10))/CNTRBASE);sendChar('0');sendChar('0');} // es 100,200,300,400,500,600,700,800,900,1100,..
                  else {setXY(13,0);sendChar('0');sendChar('0');sendChar('0');} // es 000,1000,2000,...

           lcdsegs++;        
        #endif
        #ifdef OLED1306
            if (lcdsegs % 10 != 0) {setXY(15,0);sendChar(48+lcdsegs%10);} // ultima cifra 1,2,3,4,5,6,7,8,9
            else 
               if (lcdsegs % CNTRBASE != 0){setXY(14,0);sendChar(48+(lcdsegs%CNTRBASE)/10);sendChar('0');} // es 10,20,30,40,50,60,70,80,90,110,120,..
               else 
                  if (lcdsegs % (CNTRBASE*10) != 0) {setXY(13,0);sendChar(48+(lcdsegs% (CNTRBASE*10))/CNTRBASE);sendChar('0');sendChar('0');} // es 100,200,300,400,500,600,700,800,900,1100,..
                  else {setXY(13,0);sendChar('0');sendChar('0');sendChar('0');} // es 000,1000,2000,...

           lcdsegs++;
        #endif           
        #ifdef P8544
        if (lcdsegs % 10 != 0) {itoa(lcdsegs%10,PlayBytes,10);
        lcd.setCursor(13,3);
        lcd.print(PlayBytes);} // ultima cifra 1,2,3,4,5,6,7,8,9
            else 
               if (lcdsegs % CNTRBASE != 0){itoa(lcdsegs%CNTRBASE,PlayBytes,10);
               lcd.setCursor(12,3);
               lcd.print(PlayBytes);} // es 10,20,30,40,50,60,70,80,90,110,120,..
               else 
                  if (lcdsegs % (CNTRBASE*10) != 0) {itoa(lcdsegs%(CNTRBASE*10)/CNTRBASE*100,PlayBytes,10);
                  lcd.setCursor(11,3);
                  lcd.print(PlayBytes);} // es 100,200,300,400,500,600,700,800,900,1100,..
                  else {
                    lcd.setCursor(11,3);
                    lcd.print("000");} // es 000,1000,2000,...
           lcdsegs++;
        #endif
    }
}

void lcdPercent() {  
    newpct=(100 * bytesRead)/filesize;                   
    if (currpct ==100){
        currpct= 0;
        #ifdef LCDSCREEN16x2  
          lcd.setCursor(8,0);
          lcd.print(newpct);lcd.print("%");  
        #endif
        #ifdef LCDSCREEN20x4
          lcd.setCursor(11,0);
          lcd.print(newpct);lcd.print("%");  
          //lcdSpinner(); 
        #endif
        #ifdef OLED1306
          if (newpct <10) {setXY(8,0);sendChar(48+newpct%10);}
          else
            if (newpct <100){setXY(8,0);sendChar(48+newpct/10); sendChar(48+newpct%10);}
            else {setXY(8,0);sendChar('1');sendChar('0');sendChar('0');}
          sendChar('%');
        #endif
        #ifdef OLEDSH1106
          if (newpct <10) {setXY(8,0);sendChar(48+newpct%10);}
          else
            if (newpct <100){setXY(8,0);sendChar(48+newpct/10); sendChar(48+newpct%10);}
            else {setXY(8,0);sendChar('1');sendChar('0');sendChar('0');}
          sendChar('%');
        #endif
        #ifdef P8544
          lcd.setCursor(0,3);
          lcd.print(newpct);
          lcd.print("%");               
        #endif
                             
    }
    if ((newpct >currpct)&& (newpct % 1 == 0)) {
        //PlayBytes[0]='\0'; 
        //strcat_P(PlayBytes,PSTR("Playing "));itoa(newpct,PlayBytes+8,10);strcat_P(PlayBytes,PSTR("%"));
        //printtext(PlayBytes,0);
         #ifdef LCDSCREEN16x2            
            lcd.setCursor(8,0);
            lcd.print(newpct);lcd.print("%"); 
         #endif
         #ifdef LCDSCREEN20x4            
            lcd.setCursor(11,0);
            lcd.print(newpct);lcd.print("%"); 
         #endif             
        #ifdef OLED1306
          if (newpct <10) {setXY(8,0);sendChar(48+newpct%10);}
          else
            if (newpct <100){setXY(8,0);sendChar(48+newpct/10); sendChar(48+newpct%10);}
            else {setXY(8,0);sendChar('1');sendChar('0');sendChar('0');}
          sendChar('%');
        #endif
        #ifdef OLEDSH1106
          if (newpct <10) {setXY(8,0);sendChar(48+newpct%10);}
          else
            if (newpct <100){setXY(8,0);sendChar(48+newpct/10); sendChar(48+newpct%10);}
            else {setXY(8,0);sendChar('1');sendChar('0');sendChar('0');}
          sendChar('%');
        #endif
        #ifdef P8544
          lcd.setCursor(0,3);
          lcd.print(newpct);
          lcd.print("%");  
        #endif
        
        currpct = newpct;
        //lcdSpinner();   
    } 
}

