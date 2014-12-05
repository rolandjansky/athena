/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtRODReadOut.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include <cassert>

const uint32_t MdtRODReadOut::RODstart = 0xee1234ee;
const uint32_t MdtRODReadOut::RODheadersize = 0x8;
const uint32_t MdtRODReadOut::RODversion  = 0;      // ??

MdtRODReadOut::MdtRODReadOut() :
  m_dataWord(0),
  m_subdetId(0),
  m_mrodId(0),
  m_lvl1Id(0),
  m_bcId(0),
  m_triggerTypeId(0)
{ }

MdtRODReadOut::~MdtRODReadOut()
{ }

// Decode the ROD header
void MdtRODReadOut::decodeHeader(const std::vector<uint32_t>& p)
{ 

#ifndef NDEBUG
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  svcLocator->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "MdtRODReadOut::decodeHeader");
#endif

  setZero();

  if (p[0] != RODstart) 
    {
#ifndef NDEBUG
      log << MSG::ERROR << "ROD Start of header marker not found" << endreq;
#endif
      assert(0);
    }
  if (p[1] != RODheadersize) 
    {
#ifndef NDEBUG
      log << MSG::ERROR << "ROD header size doesn't match " << RODheadersize << endreq;
#endif
      assert(0);
    }
  
  // decode the rest of the header
  // Subdetector Id and mrodId
  m_word = p[3];
  m_subdetId = getBits(15,8);
  m_mrodId   = getBits(7,0);
  
  // Lvl1Id
  m_word = p[4];
  m_lvl1Id   = getBits(23,0);

  // Bunch crossing identifier
  m_word = p[5];
  m_bcId     = getBits(11,0);

  // Trigger type Id
  m_word = p[6];
  m_triggerTypeId = getBits(7,0);
  
}

// Decode the ROD footer
void MdtRODReadOut::decodeFooter(const std::vector<uint32_t>& /*p*/)
{ 
  
}

uint32_t MdtRODReadOut::makeRODId(uint16_t subdet, uint16_t mrod)
{ 
  uint16_t inputData[4] = {  0,  0, subdet, mrod};
  uint16_t inputPos[4]  = { 24, 16,      8,    0};
  uint16_t nData = 4;

  return setBits(nData,inputData,inputPos);
}

uint32_t* MdtRODReadOut::encodeFooter()
{

  return 0;
}

void MdtRODReadOut::setZero()
{
  m_subdetId = 0;   
  m_mrodId = 0;     
  m_lvl1Id = 0;     
  m_bcId = 0;       
  m_triggerTypeId = 0;
}

