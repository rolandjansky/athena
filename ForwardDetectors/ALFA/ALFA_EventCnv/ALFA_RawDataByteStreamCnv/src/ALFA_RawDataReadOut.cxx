/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataReadOut.h"

ALFA_RawDataReadOut::ALFA_RawDataReadOut() :
  m_dataWord(0),
  m_wordHeader(0),
  m_SlotId(0),
  m_LsbTdcId(0),
  m_ecnt_BOT(0),
  m_ecnt_EOT(0),
  m_bcId(0),
  m_wcnt(0),
  m_MBId(0),
  m_bit26_27(0),
  m_bit24_27(0),	
  m_WordId(0),
  m_PMFId(0),
  m_fiber(0),
  m_ADC(0),
  m_pattern(),
  m_scaler(0),
  m_bit16(0),
  m_error_bit17(0),
  m_bit18(0)
{  }

ALFA_RawDataReadOut::~ALFA_RawDataReadOut()
{ }

void ALFA_RawDataReadOut::decodeWord(uint32_t dataWord)

{ 

  //  Zero all the decoded quantities
  setZero();
  m_dataWord = dataWord;
  m_word = dataWord;
  m_wordHeader = (dataWord>>s_headerPos)&s_headerBits;

  if (is_TDCt())         // TDC single measurement
    {
      m_bit18 = (bool) getBits(18,18);
      m_error_bit17 = (bool) getBits(17,17);
      m_WordId      = getBits(25,24);
      m_PMFId = getBits(23,19);
      m_bit16 = (bool) getBits(16,16);
      m_bit26_27 = getBits(27,26);
      m_fiber  = getBits(15,0);
      m_ADC  = getBits(11,0); // PMF24: Word 2 -> ADC 1, Word 3 -> ADC 2

      for(int i = 15; i > -1; i--)  // PMF 24 Word 0 bits from 15 to 0 contain trigger information (current and previous 3 bunch crossings)
	{
	  if(getBits(i,i)) m_pattern.push_back(true);
	  else m_pattern.push_back(false);
	}

      m_scaler  = getBits(15,0);  // PMF 24 Word 1
      m_MBId   = getBits(7,0); // PMF0 - WordId 0 -> position from 1-15
    }
	
  	
  else if (is_BOT())               // Beginning of TDC
    { 
      // One header bit is used for TDC numbers > 15
      m_SlotId   = getBits(28,24);
      m_ecnt_BOT    = getBits(23,12);
      m_bcId    = getBits(11,0);
    }
  else if (is_EOT())          // End of TDC
    {
      m_LsbTdcId = getBits(25,24);
      m_ecnt_EOT    = getBits(23,12);
      m_wcnt    = getBits(11,0);
      m_bit24_27 = getBits(27,24);
    }
    
    
	  
}


void ALFA_RawDataReadOut::setZero()
{
  m_SlotId = 0;
  m_LsbTdcId = 0;
  m_ecnt_BOT  = 0;
  m_ecnt_EOT  = 0;
  m_bcId  = 0;
  m_wcnt  = 0;
  m_WordId    = 0;
  m_PMFId=0;
  m_fiber =0;
  m_ADC=0;
  m_pattern.clear();
  m_scaler=0;
  m_bit16=false;
  m_error_bit17=false;
  m_bit18=false;
  m_MBId = 0;   
  m_bit26_27=0;
  m_bit24_27=0; 
}

//-------------------------------------------------------------------------
