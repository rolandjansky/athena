/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtHptdcReadOut.h"

//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/Bootstrap.h"
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/IMessageSvc.h"


MdtHptdcReadOut::MdtHptdcReadOut() :
  m_dataWord(0), m_wordHeader(0), m_tdcId(0), m_ecnt(0), m_bcId(0),
  m_wcnt(0), m_channel(0), m_coarse(0), m_fine(0), m_width(0),
  m_errflag(0), m_leading(false)
{  }


MdtHptdcReadOut::~MdtHptdcReadOut()
{ }

void MdtHptdcReadOut::decodeWord(uint32_t dataWord)
{
  //  Zero all the decoded quantities
  setZero();
  m_dataWord = dataWord;
  m_word = dataWord;
  m_wordHeader = (dataWord>>HEADERPOS)&HEADERBITS;

  if (is_TSM())         // TDC single measurement
    {
      m_leading = ! ((bool) getBits(28,28));
      m_tdcId   = getBits(27,24);
      m_channel = getBits(23,19);
      m_coarse  = getBits(18,5);
      m_fine    = getBits(4,0);
    }
  else if (is_TCM())         // TDC combined measurement
    {
      m_tdcId   = getBits(27,24);
      m_channel = getBits(23,19);
      m_width   = getBits(18,12);
      m_coarse  = getBits(11,5);
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
      m_tdcId   = getBits(27,24);
      m_ecnt    = getBits(23,12);
      m_wcnt    = getBits(11,0);
    }
  else if (is_TES())        // TDC error status
    {
      m_tdcId   = getBits(27,24);
      m_errflag = getBits(14,0);
    }
}


void MdtHptdcReadOut::setZero()
{
  m_tdcId = 0;
  m_ecnt  = 0;
  m_bcId  = 0;
  m_wcnt  = 0;
  m_channel=0;
  m_coarse =0;
  m_fine  = 0;
  m_width = 0;
  m_errflag=0;
  m_leading=false;
}

// Encoding methods
// Beginning of TDC
uint32_t MdtHptdcReadOut::makeBOT(uint16_t tdcId, uint16_t ecnt, uint16_t bcid)
{
  uint16_t inputData[4];
  uint16_t inputPos[4] = {HEADERPOS, 24, 12, 0};
  uint16_t nData = 4;

  if (tdcId < 16) {
    inputData[0]=BOTVALUE1;
  }
  else {
    inputData[0]=BOTVALUE2;
    tdcId -= 16;
  }

  inputData[1] = tdcId;
  inputData[2] = ecnt;
  inputData[3] = bcid;

  return setBits(nData,inputData,inputPos);
}

// End of TDC
uint32_t MdtHptdcReadOut::makeEOT(uint16_t tdcId, uint16_t ecnt, uint16_t wcnt)
{
  uint16_t inputData[4] = {EOTVALUE,  tdcId, ecnt, wcnt};
  uint16_t inputPos[4]  = {HEADERPOS, 24, 12, 0};
  uint16_t nData = 4;

  return setBits(nData,inputData,inputPos);
}

// Make a TDC single measurement word
uint32_t MdtHptdcReadOut::makeTSM(uint16_t tdcId, uint16_t channel, bool leading,
				  uint16_t coarse, uint16_t /*fine*/)
{

  uint16_t TSMvalue = leading ? TSMVALUE_LEAD : TSMVALUE_TAIL;

  uint16_t inputData[4] = {TSMvalue ,
			   static_cast<uint16_t> (tdcId   & 0xf),
			   static_cast<uint16_t> (channel & 0x1f),
			   static_cast<uint16_t> (coarse  & 0x7ffff)};
  uint16_t inputPos[4]  = {HEADERPOS, 24, 19, 0};
  uint16_t nData = 4;

  return setBits(nData,inputData,inputPos);
}

// Make a TDC combined measurement word
uint32_t MdtHptdcReadOut::makeTCM(uint16_t tdcId, uint16_t channel, uint16_t width,
		   uint16_t coarse, uint16_t /*fine*/)
{
  uint16_t inputData[5] = {TCMVALUE,
			   static_cast<uint16_t> (tdcId   & 0xf),
			   static_cast<uint16_t> (channel & 0x1f),
			   static_cast<uint16_t> (width   & 0x7f),
			   static_cast<uint16_t> (coarse  & 0xfff) };
  uint16_t inputPos[5]  = {HEADERPOS, 24, 19, 12, 0};
  uint16_t nData = 5;

  return setBits(nData,inputData,inputPos);
}

