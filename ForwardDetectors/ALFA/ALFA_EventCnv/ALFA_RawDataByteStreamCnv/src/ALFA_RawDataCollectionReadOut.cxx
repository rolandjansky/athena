/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataCollectionReadOut.h"


ALFA_RawDataCollectionReadOut::ALFA_RawDataCollectionReadOut():
  m_dataWord(0),
  m_wordHeader(0),
  m_wordHeader2(0),
  m_lvl1Id(0),
  m_clwc(0),
  m_ftyp(0),
  m_ecnt_LWC(0),
  m_ecnt_TWC(0),
  m_ecnt_EOB(0),
  m_ctwc(0),
  m_BOLflag(0),
  m_MrodNum(0),
  m_RODinput(0),
  m_KaptId(0)
{ }


ALFA_RawDataCollectionReadOut::~ALFA_RawDataCollectionReadOut()
{ }

void ALFA_RawDataCollectionReadOut::decodeWord(uint32_t dataWord)

{
  setZero();

  m_dataWord   = dataWord;
  m_word       = dataWord;
  m_wordHeader = (dataWord>>s_headerPos) & s_headerBits;
  m_wordHeader2 = (dataWord>>s_headerPos2) & s_headerBits2;

  if (is_BOB())
    {
      m_lvl1Id = getBits(23,0);
    }
  else if (is_LWC())
    {
      m_ecnt_LWC = getBits(19,16);
      m_clwc = getBits(15,0);
    }
  else if (is_BOL())
    {
      m_BOLflag   = getBits(23,16);
      m_MrodNum   = getBits(15,4);
      m_RODinput    = getBits(3,0);
    }
  else if (is_TLP())
    {
      m_KaptId = getBits(19,0);
    }
  else if (is_TWC())
    {
      m_ecnt_TWC  = getBits(23,12);
      m_ctwc  = getBits(11,0);
    }
  else if (is_EOB())
    {
      m_ecnt_EOB  = getBits(15,0);
    }
    
       
}


void ALFA_RawDataCollectionReadOut::setZero()
{
  m_lvl1Id = 0;   // Lvl1 Id
  m_clwc = 0;     // Link word count
  m_ftyp = 0;     // flag type
  m_KaptId = 0;     // flags
  
  m_BOLflag= 0;
  
  m_ecnt_LWC = 0;     // Event counter
  m_ecnt_TWC = 0;     // Event counter 
  m_ecnt_EOB = 0;     // Event counter 
  
  m_ctwc = 0;     // Trailer word count
  
  m_MrodNum = 0;   // MROD number
  m_RODinput = 0;   // ROD input
  
  

}


