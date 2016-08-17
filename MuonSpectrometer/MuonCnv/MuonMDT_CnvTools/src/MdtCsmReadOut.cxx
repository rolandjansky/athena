/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCsmReadOut.h"

//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/Bootstrap.h"
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/IMessageSvc.h"

MdtCsmReadOut::MdtCsmReadOut():
  m_dataWord(0),
  m_wordHeader(0),
  m_lvl1Id(0),
  m_clwc(0),
  m_ftyp(0),
  m_flags(0),
  m_ecnt(0),
  m_ctwc(0),
  m_mrodId(0),
  m_csmId(0),
  m_error(0),
  m_zeroSupp(0)
{ }


MdtCsmReadOut::~MdtCsmReadOut()
{ }

void MdtCsmReadOut::decodeWord(uint32_t dataWord)
{
  setZero();

  m_dataWord   = dataWord;
  m_word       = dataWord;
  m_wordHeader = (dataWord>>s_headerPos) & s_headerBits;

  if (is_BOB())
    {
      m_lvl1Id = getBits(23,0);
    }
  else if (is_LWC())
    {
      m_ftyp = getBits(23,20);
      m_clwc = getBits(19,0);
    }
  else if (is_BOL())
    {
      m_error    = (bool) getBits(20,20);
      m_zeroSupp = (bool) getBits(16,16);
      m_mrodId   = getBits(15,4);
      m_csmId    = getBits(3,0);
    }
  else if (is_TLP())
    {
      m_flags = getBits(17,0);
    }
  else if (is_TWC())
    {
      m_ecnt  = getBits(23,12);
      m_ctwc  = getBits(11,0);
    }
}

uint32_t MdtCsmReadOut::makeBOB(uint32_t lvl1Id)
{
  uint16_t nData = 2;
  uint32_t inputData[2] = {s_BOBvalue, lvl1Id};
  uint16_t inputPos[2] = {s_headerPos,      0};
  
  return setBits(nData, inputData, inputPos);
}

uint32_t MdtCsmReadOut::makeLWC(uint32_t clwc)
{
  uint16_t nData = 3;
  uint32_t inputData[3] = {s_LWCvalue,  0, clwc};
  uint16_t inputPos[3] = {s_headerPos, 20,   0};
  
  return setBits(nData, inputData, inputPos);
}

uint32_t MdtCsmReadOut::makeBOL(uint16_t mrodId, uint16_t csmId)
{
  uint16_t nData = 5;
  // Error and Zero-suppression bits are by now set to 0 (no error)
  // and 1 (Zero-suppression activated)
  uint32_t inputData[5] = {s_BOLvalue, 0, 1, mrodId, csmId};
  uint16_t inputPos[5] = {s_headerPos,20,16,      4,     0};

  return setBits(nData, inputData, inputPos);
}

uint32_t MdtCsmReadOut::makeTLP(uint32_t flags)
{
  uint16_t nData = 4;
  uint32_t inputData[4] = {s_TLPvalue,  0,  0, flags};
  uint16_t inputPos[4] = {s_headerPos, 20, 18,     0};

  return setBits(nData, inputData, inputPos);  
}

uint32_t MdtCsmReadOut::makeTWC(uint16_t ecnt, uint16_t ctwc)
{
  uint16_t nData = 3;
  uint16_t inputData[3] = {s_TWCvalue, ecnt, ctwc};
  uint16_t inputPos[3] = {s_headerPos,   12,    0};

  return setBits(nData, inputData, inputPos);    
}

uint32_t MdtCsmReadOut::makeEOB(uint32_t wcnt)
{
  uint16_t nData = 2;
  uint32_t inputData[2] = {s_EOBvalue, wcnt};
  uint16_t inputPos[2] = {s_headerPos,    0};

  return setBits(nData, inputData, inputPos);      
}

void MdtCsmReadOut::setZero()
{
  m_lvl1Id = 0;   // Lvl1 Id
  m_clwc = 0;     // Link word count
  m_ftyp = 0;     // flag type
  m_flags = 0;     // flags
  
  m_ecnt = 0;     // Event counter
  m_ctwc = 0;     // Trailer word count
  
  m_mrodId = 0;   // MROD number
  m_csmId = 0;    // CSM Link number
  m_error = false;      // Error flag
  m_zeroSupp = false;   // TDC zero-suppressed flag
}


