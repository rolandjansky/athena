/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtAmtReadOut.h"

//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/Bootstrap.h"
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/IMessageSvc.h"


MdtAmtReadOut::MdtAmtReadOut() :
  m_dataWord(0), m_wordHeader(0), m_tdcId(0), m_ecnt(0), m_bcId(0),
  m_wcnt(0), m_jt(0), m_channel(0), m_coarse(0), m_fine(0), m_width(0),
  m_errflag(0), m_errstatus(0), m_masked(0), m_leading(false)
{  }


MdtAmtReadOut::~MdtAmtReadOut()
{ }

void MdtAmtReadOut::decodeWord(uint32_t dataWord)
{ 
  //  Zero all the decoded quantities
  setZero();
  m_dataWord = dataWord;
  m_word = dataWord;
  m_wordHeader = (dataWord>>s_headerPos)&s_headerBits;

  if (is_TSM())         // TDC single measurement
    {
      m_leading = (bool) getBits(18,18);
      m_jt      = getBits(25,24);
      m_channel = getBits(23,19);
      m_errflag = getBits(17,17);
      m_coarse  = getBits(16,5);
      m_fine    = getBits(4,0);
    }
  else if (is_TCM())         // TDC combined measurement
    {
      m_jt      = getBits(25,24);
      m_channel = getBits(23,19);
      m_width   = getBits(18,11);
      m_coarse  = getBits(10,5);
      m_fine    = getBits(4,0);
    }
  else if (is_BOT())               // Beginning of TDC
    { 
      // One header bit is used for TDC numbers > 15
      m_tdcId   = getBits(28,24);
      m_ecnt    = getBits(23,12);
      m_bcId    = getBits(11,0);
    }
  else if (is_EOT())          // End of TDC
    {
      m_ecnt    = getBits(23,12);
      m_wcnt    = getBits(11,0);
    }
  else if (is_TMC())         // TDC masked channels flag
    {
      m_jt      = getBits(25,24);
      m_masked  = getBits(23,0);
    }
  else if (is_TES())        // TDC error status
    {
      m_jt      = getBits(25,24);
      m_errflag = getBits(6,0);
    }
  // special decoding of the CSM trailer word count - nothing to do here
  else if (is_TWC())
    {

    }
}


void MdtAmtReadOut::setZero()
{
  m_tdcId = 0;
  m_ecnt  = 0;
  m_bcId  = 0;
  m_wcnt  = 0;
  m_jt    = 0;
  m_channel=0;
  m_coarse =0;
  m_fine  = 0;
  m_width = 0;
  m_errflag=0;
  m_errstatus=0;
  m_masked=0;
  m_leading=false;
}

// Encoding methods
// Beginning of TDC
uint32_t MdtAmtReadOut::makeBOT(uint16_t tdcId, uint16_t ecnt, uint16_t bcid)
{ 
  uint16_t inputData[4];
  uint16_t inputPos[4] = {s_headerPos, 24, 12, 0};
  uint16_t nData = 4;

  if (tdcId < 16) {
    inputData[0]=s_BOTvalue1;
  }
  else {
    inputData[0]=s_BOTvalue2;
    tdcId -= 16;
  }

  inputData[1] = tdcId;
  inputData[2] = ecnt;
  inputData[3] = bcid;

  return setBits(nData,inputData,inputPos);
}

// End of TDC
uint32_t MdtAmtReadOut::makeEOT(uint16_t jt, uint16_t ecnt, uint16_t wcnt)
{ 
  uint16_t inputData[5] = {s_EOTvalue,   0, jt, ecnt, wcnt};
  uint16_t inputPos[5]  = {s_headerPos, 26, 24, 12, 0};
  uint16_t nData = 5;

  return setBits(nData,inputData,inputPos);
}

// Make a TDC single measurement word
uint32_t MdtAmtReadOut::makeTSM(uint16_t jt, uint16_t channel, bool leading, 
				bool errflag, uint16_t coarse, uint16_t fine)
{ 
  uint16_t lead = 1;
  uint16_t err  = 0;
  if (leading) lead = 0;
  if (errflag) err  = 1;

  uint16_t inputData[8] = {(uint16_t)s_TSMvalue ,  
                           0, 
			   static_cast<uint16_t> (jt      & 0x2), 
			   static_cast<uint16_t> (channel & 0x1f), 
			   static_cast<uint16_t> (lead    & 0x1), 
			   static_cast<uint16_t> (err     & 0x1), 
			   static_cast<uint16_t> (coarse  & 0xfff), 
			   static_cast<uint16_t> (fine    & 0x1f)};
  uint16_t inputPos[8]  = {s_headerPos, 26, 24,      19,   18,  17,      5,    0};
  uint16_t nData = 8;

  return setBits(nData,inputData,inputPos);
}

// Make a TDC combined measurement word
uint32_t MdtAmtReadOut::makeTCM(uint16_t jt, uint16_t channel, uint16_t width,
		   uint16_t coarse, uint16_t fine)
{
  uint16_t inputData[7] = {(uint16_t)s_TCMvalue ,  
                           0, 
			   static_cast<uint16_t> (jt      & 0x2), 
			   static_cast<uint16_t> (channel & 0x1f), 
			   static_cast<uint16_t> (width   & 0xff), 
			   static_cast<uint16_t> (coarse  & 0x3f), 
			   static_cast<uint16_t> (fine    & 0x1f) };
  uint16_t inputPos[7]  = {s_headerPos, 26, 24,      19,    11,      5,    0};
  uint16_t nData = 7;

  return setBits(nData,inputData,inputPos);
}

uint32_t MdtAmtReadOut::makeTMC(uint16_t jt, uint32_t masked)
{

  uint16_t masked_low  = masked & 0xffff;
  uint16_t masked_high = masked & 0xff0000;

  uint16_t inputData[5] = {s_TMCvalue ,  0, jt, masked_high, masked_low};
  uint16_t inputPos[5]  = {s_headerPos, 26, 24, 16,  0};
  uint16_t nData = 5;

  return setBits(nData,inputData,inputPos);  
}
