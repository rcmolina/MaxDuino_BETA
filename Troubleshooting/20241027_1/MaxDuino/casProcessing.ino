#ifdef Use_CAS

void casPause()
{
  noInterrupts();
  isStopped=pauseOn;
  interrupts();
}

void bits_to_pulses()
{
  // converts from a packed representation of bits to output (bitword)
  // into a sequence of high and low output levels to emit
  //
  // needs some explanation:
  // bitword will either be 0x8000 to indicate "output silence"
  // or a set of bits (usually 8 or 11 bits), LSB first,
  // e.g. xxxxx11101010100
  //
  // A bit (0 or 1) will be mapped to a set of pulses:
  // 0: a "0" is output as one long pulse        ___/"""
  // 1: a "1" is output as two short pulses      _/"\_/"
  //    or if file is a DRAGONMODE file
  //    then ONE short pulse                     _/"
  //
  // However, note that all the above are inverted for DRAGNOMODE
  // 0:                                          """\___
  // 1:                                          "\_
  //
  // lastly, silence (instead of a bit) is output as a continuous low value
  // repeated for 4 half-periods:                ________
  //
  // or, for DRAGONMODE, as a high value:        """"""""
  //
  byte val;
  if (bitword & 0x8000)
  {
    // signifies 'force low', rather than a bit to be output as a pattern
    if (!out) val=0;
    else val=1;
  }
  else
  {
    switch (bitword & 0x0001) {
      case 0:
        if(pass==0 || pass==1) {
          if (!out) val=0;
          else val=1;
        } else {
          if (!out) val=1;
          else val=0; 
        }
        break;

      case 1:
        if(pass==0 || pass==2) {
          if (!out) val=0;    
          else val=1;
        } else {
          if (!out) val=1;
          else val=0;
        }
        #if defined(Use_DRAGON)
        if(casduino == CASDUINO_FILETYPE::DRAGONMODE && pass == 1) {
          pass=3;
        }
        #endif
        break;
    }
  }

  if(writepos==0)
  {
    // first time (or once per buffer-fill), set sample period.
    const word _currentPeriod = period | 0x6000;
    const byte _b1 = _currentPeriod /256;
    const byte _b2 = _currentPeriod %256;
    volatile byte * _wb = writeBuffer+writepos;
    noInterrupts();                       //Pause interrupts while we add a period to the buffer
    *_wb = _b1;
    *(_wb+1) = _b2;
    interrupts();
    writepos+=2;
    return;
  }
  else
  {
    const byte _b2 = val?128:0; // just one bit
    volatile byte * _wb = writeBuffer+writepos;
    noInterrupts();                       //Pause interrupts while we add a period to the buffer
    *_wb = 0x40; // = (1<<14)>>8;
    *(_wb+1) = _b2;
    interrupts();
    writepos+=2;
  }

  pass++;
  if(pass == 4) 
  {
    // all pulses delivered for this bit so on to the next bit
    pass=0;
    currentBit--;
    if (!(bitword & 0x8000))
      bitword >>= 1;
  }
}

void writeByte(byte b)
{
  // 'casduino' holds the number of bits: 8 (DRAGON) or 11 (CAS)
  currentBit = (byte)casduino;
#if defined(Use_DRAGON)
  if(casduino == CASDUINO_FILETYPE::CASDUINO)
#endif
  {
    // bitword = xxxxx11bbbbbbbb0
    //                ^^\______/^
    //                 |  data  |
    //                 |        |
    //                 2 stop   1 start
    //                   bits     bit
    //                   = 11     = 0
    //
    bitword = (((word)b)<<1) + 0x0600;
  }
#if defined(Use_DRAGON)
  else
  {
    // bitword = xxxxxxxxbbbbbbbb
    //                   \______/
    //                     data
    bitword = b;
  }
#endif
}

void writeSilence()
{
  // write a 'low' output
  bitword = 0x8000;
  currentBit = (byte)casduino;
}

void writeHeader()
{
  // write a header of 1 bits
  bitword = 0x07ff;  // 11 1's i.e. 0b0000011111111111
  currentBit = (byte)casduino;
}

void process()
{
  if(cas_currentType==CAS_TYPE::typeEOF)
  {
    if(count) {
      writeSilence();
      count--;
    } else stopFile();    
    return;
  }
  if(currentTask==TASK::GETFILEHEADER || currentTask==TASK::CAS_wData)
  {
    if(readfile(8,bytesRead))
    {
      if(!memcmp_P(filebuffer, HEADER,8)) {
        if(fileStage==0) 
        {
          currentTask = TASK::CAS_lookType;
        } else
        {
          currentTask = TASK::CAS_wSilence;
          count=SHORT_SILENCE*scale;
        }
        if(cas_currentType==CAS_TYPE::Nothing) fileStage=1;
        bytesRead+=8;
      }
    } else
    {
      cas_currentType=CAS_TYPE::typeEOF;
      currentTask=TASK::CAS_wClose;
      count=LONG_SILENCE*scale;
    }
     
  }

  if(currentTask==TASK::CAS_lookType)
  {
    currentTask = TASK::CAS_wSilence;
    count = LONG_SILENCE*scale;
    fileStage=1;       
    cas_currentType = CAS_TYPE::Unknown;
//    if((r=readfile(10,bytesRead))==10)
    if((readfile(10,bytesRead))==10)
    {
      if(cas_file_match(CAS_ASCII))
      {
        cas_currentType = CAS_TYPE::Ascii;
      }else if(cas_file_match(CAS_BINF))
      {
        cas_currentType = CAS_TYPE::Binf;
      }else if(cas_file_match(CAS_BASIC))
      {
        cas_currentType = CAS_TYPE::Basic;
      }
    }
  }

  if(currentTask==TASK::CAS_wSilence)
  {
    if(count)
    {
      writeSilence();
      count--;
    } else 
    {
      currentTask=TASK::CAS_wHeader;
      if(fileStage==1) 
      {
        //count=LONG_HEADER*scale;
        count=LONG_HEADER; 
        fileStage+=1;
      } else 
      {
        count=SHORT_HEADER*scale;
        //count=SHORT_HEADER;
        if(cas_currentType==CAS_TYPE::Ascii) {
          fileStage+=1;
        } else {
          fileStage=0;
        }
      }
    }
  }
  if(currentTask==TASK::CAS_wHeader)
  {
    if(count)
    {
      writeHeader();
      count--;
    } else
    {
      currentTask=TASK::CAS_wData;
      return;
    }
  }
  if(currentTask==TASK::CAS_wData)
  {
    writeByte(filebuffer[0]);
    if(filebuffer[0]==0x1a && cas_currentType==CAS_TYPE::Ascii) 
    {
      fileStage=0;
    }
  }
  if(currentTask==TASK::GETFILEHEADER || currentTask==TASK::CAS_lookType || currentTask==TASK::CAS_wData) bytesRead+=1; 
}

bool cas_file_match(const byte matchval)
{
  // simply return true if all bytes between filebuffer and filebuffer+9
  // match the matchval
  for (byte i=9; i>=0; --i)
    if (filebuffer[i] != matchval) return false;
  return true;
}

#if defined(Use_DRAGON)
void processDragon()
{
  lastByte=filebuffer[0];
//  byte r=0;
//  if((r=readfile(1,bytesRead))==1) {
  if((readfile(1,bytesRead))==1) {

  #if defined(Use_Dragon_sLeader) && not defined(Expand_All)
    if(currentTask==TASK::GETFILEHEADER) {
      if(filebuffer[0] == 0x55) {
        writeByte(0x55);
        bytesRead+=1;
        if(count) count--;
      } else {
       //currentTask=TASK::CAS_wHeader;
        if(count) {
          writeByte(0x55);
          count--;
        } else {    
          if (fileStage > 0) currentTask=TASK::CAS_wData;
          else {
            count =19;
            currentTask=TASK::CAS_wNameFileBlk;
          }
        }       
      }
    } else if(currentTask==TASK::CAS_wNameFileBlk) {
      if(count) {
        writeByte(filebuffer[0]);
        bytesRead+=1;
        count--;            
      } else {            
        fileStage=1;
        currentTask=TASK::GETFILEHEADER;
        count=255;
      }
    } else {        
  #endif
    
  #if defined(Use_Dragon_sLeader) && defined(Expand_All)

    if(currentTask==TASK::GETFILEHEADER) {
      if(filebuffer[0] == 0x55) {
        writeByte(0x55); 
        bytesRead+=1;
        if(count) count--;
      } else {
       //currentTask=TASK::CAS_wHeader;
        if(count) {
          writeByte(0x55);
          count--;
        } else {
          //count= 119;
          count = 2;      
          currentTask=TASK::CAS_wSync;
        }       
      }
    } else if(currentTask==TASK::CAS_wSync) { 
      if(count) {
        writeByte(filebuffer[0]);
        bytesRead+=1;
        count--;
      } else {
        writeByte(filebuffer[0]);            // block length
        bytesRead+=1;
        currentTask=TASK::CAS_wNameFileBlk;
        count=filebuffer[0]++;                   
      }
 
    } else if(currentTask==TASK::CAS_wNameFileBlk) { 
      if(count) {
        writeByte(filebuffer[0]);
        bytesRead+=1;
        count--;
      } else {
        writeByte(filebuffer[0]);            //Si no cierras el FileNmae block con el primer 0x55 se desincroniza
        bytesRead+=1;            
        currentTask=TASK::CAS_lookLeader;
        count=255;                 
      }
          
    } else if(currentTask==TASK::CAS_lookLeader) { 
      if(filebuffer[0] == 0x55) {
        writeByte(0x55); 
        bytesRead+=1;
        if(count) count--;
      } else {
        //currentTask=TASK::CAS_wNewLeader;
        if(count) {
          writeByte(0x55);
          count--;
        } else {   
          currentTask=TASK::CAS_wData;
        }
        
      }
    } else {              // data block

  #endif
      //currentTask=TASK::CAS_wData;
      writeByte(filebuffer[0]);
      bytesRead+=1;             // increase pointer to read next byte (not the same)
  //#if defined(Use_DRAGON) && defined(Use_Dragon_sLeader)                       
    }
  //#endif
  
    } else {          // readfile !=1 , ending
      if(currentTask==TASK::CAS_wData) {
       if(lastByte != 0x55) {
          writeByte(0x55);
      }      
      count = 54;
      currentTask=TASK::CAS_wSilence;
    }    
    if(currentTask==TASK::CAS_wSilence) {
      if(count) {
        writeSilence();
        count--;
      } else {
        stopFile();
      }
    }
  }
}
#endif

void casduinoLoop()
{
  noInterrupts();
  isStopped=pauseOn;
  if(morebuff)
  {
    //Buffer has swapped, start from the beginning of the new page
    writepos=0;
    morebuff=false;    
  }
  interrupts();

  if(currentBit==0)
  { 
#if defined(Use_DRAGON)
    if(casduino == CASDUINO_FILETYPE::DRAGONMODE) {
      processDragon();
    }
    else
#endif
    {
      process();      
    }
  }

  if(writepos<buffsize)
  {
    while(writepos<buffsize && currentBit)
    {
      bits_to_pulses();
    }        
  } else {
    if (!pauseOn) {      
    #if defined(SHOW_CNTR)
      lcdTime();          
    #endif
    #if defined(SHOW_PCT)          
      lcdPercent();
    #endif        
    }
  } 
}

#endif
