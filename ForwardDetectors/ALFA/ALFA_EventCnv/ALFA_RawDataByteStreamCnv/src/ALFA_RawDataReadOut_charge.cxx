/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataReadOut_charge.h"

ALFA_RawDataReadOut_charge::ALFA_RawDataReadOut_charge() :
  m_dataWord(0),
  m_wordHeader(0),
  m_SlotId(0),
  m_LsbTdcId(0),
  m_ecnt_BOT(0),
  m_ecnt_EOT(0),
  m_bcId(0),
  m_wcnt(0),
  m_MBId(0),
  m_FiberFirmwareId(0),
  m_PMFId(0),
  m_ChannelNumId(0),
  m_ChargeChanId(0),
  m_bit12(0),
  m_bit24_27(0),	
  m_bit27_24(0),
  m_ChargeChan()
{  }

ALFA_RawDataReadOut_charge::~ALFA_RawDataReadOut_charge()
{ }

void ALFA_RawDataReadOut_charge::decodeWord(uint32_t dataWord)

{ 

  //  Zero all the decoded quantities
  setZero();
  m_dataWord = dataWord;
  m_word = dataWord;
  m_wordHeader = (dataWord>>s_headerPos)&s_headerBits;

  if (is_TDCch())         // TDC single measurement
    {
      
      m_PMFId      = getBits(getBitsWord(23,19));
      m_ChannelNumId = getBits(getBitsWord(18,13));
      m_bit12 = (bool) getBits(getBitsWord(12,12));
      m_ChargeChanId  = getBits(getBitsWord(11,0));
      
      m_MBId   = getBits(getBitsWord(7,0)); // PMF0 - PMFId 0 -> position from 1-15
      m_FiberFirmwareId = getBits(getBitsWord(15,8)); // PMF0 - PMFId 0 
      
      m_bit27_24 = getBits(getBitsWord(27,24));
      
    }
	
  	
  else if (is_BOT())               // Beginning of TDC
    { 
      // One header bit is used for TDC numbers > 15
      m_SlotId   = getBits(getBitsWord(28,24));
      m_ecnt_BOT    = getBits(getBitsWord(23,12));
      m_bcId    = getBits(getBitsWord(11,0));
    }
  else if (is_EOT())          // End of TDC
    {
      m_LsbTdcId = getBits(getBitsWord(25,24));
      m_ecnt_EOT    = getBits(getBitsWord(23,12));
      m_wcnt    = getBits(getBitsWord(11,0));
      m_bit24_27 = getBits(getBitsWord(27,24));
    }
    
    
	  
}


void ALFA_RawDataReadOut_charge::setZero()
{
  m_SlotId = 0;
  m_LsbTdcId = 0;
  m_ecnt_BOT  = 0;
  m_ecnt_EOT  = 0;
  m_bcId  = 0;
  m_wcnt  = 0;
  m_PMFId    = 0;
  m_FiberFirmwareId =  0;
  m_ChannelNumId=0;
  m_ChargeChanId =0;
  m_bit12=false;
  m_MBId = 0;   
  m_bit24_27=0; 
  m_bit27_24=0; 
  
}

//-------------------------------------------------------------------------
