/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataContainerReadOut.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include <cassert>

 
ALFA_RawDataContainerReadOut::ALFA_RawDataContainerReadOut():
  m_wordMarker(0),
  m_subdetId(0),
  m_mrodId(0),
  m_lvl1Id(0),
  m_ecrId(0),
  m_bcId(0),
  m_runNum(0),
  m_runType(0),
  m_triggerTypeId(0),
  m_DetEventType(0),
  m_timestamp(0),
  m_BCId(0)
{ }

ALFA_RawDataContainerReadOut::~ALFA_RawDataContainerReadOut()
{ }

void ALFA_RawDataContainerReadOut::decodeWord(uint32_t dataWord)
{
  m_wordMarker = dataWord;
}

void ALFA_RawDataContainerReadOut::decodeHeaderFull(std::vector<uint32_t>& vDataFull)
{

  setZeroFull();

 
  if (vDataFull[0] == s_FullEVmarker) 
    {
      // cout  << "Full HEADER found : vDataFull[0]  "<< std::hex  << vDataFull[0] <<  std::dec  <<endl;

    }
    
  m_word = vDataFull[8];
  m_timestamp = getBits(31,0); 
  // cout  << " vDataFull[8]  " << std::hex << vDataFull[8]  << " m_timestamp  " << std::hex << m_timestamp <<std::dec<<endl;
 
  m_word = vDataFull[15];
  m_BCId = getBits(31,0); 
  // cout  << " vDataFull[15]  " << std::hex << vDataFull[15]  << " m_BCId  " << std::hex << m_BCId <<std::dec<<endl;
 
}


// Decode the ROD header
void ALFA_RawDataContainerReadOut::decodeHeader(std::vector<uint32_t>& vData)
{ 

  //static const uint32_t RODheadersize = 0x00000009; 


#ifndef NDEBUG
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  svcLocator->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "ALFA_RawDataContainerReadOut::decodeHeader");
#endif

  setZero();

  if (vData[0] != s_RODmarker) 
    {
#ifndef NDEBUG
      log << MSG::ERROR << "ROD Start of header marker not found" << endmsg;
#endif
      assert(0);
    }
  if (vData[1] != s_RODheadersize) 
    {
#ifndef NDEBUG
      log << MSG::ERROR << "ROD header size doesn't match " << endmsg;
#endif
      assert(0);
    }
  
  // decode the rest of the header
  // Subdetector Id and mrodId
  m_word = vData[3];
  m_subdetId = getBits(31,16); 
  m_mrodId   = getBits(15,0);
  
  
  // Run number
  m_word = vData[4];
  m_runNum = getBits(23,0); 
  m_runType   = getBits(31,24);
 
 
  // Lvl1Id :counting events
  m_word = vData[5];
  m_lvl1Id   = getBits(23,0);
  m_ecrId = getBits(31,24);
 
  	
  // Bunch crossing identifier
  m_word = vData[6];
  m_bcId     = getBits(11,0);
 

  // Trigger type Id
  m_word = vData[7];
  m_triggerTypeId = getBits(7,0);
 
  // Detector event type 
  m_word = vData[8];
  m_DetEventType = getBits(23,0);
}




void ALFA_RawDataContainerReadOut::setZero()
{
  m_subdetId = 0;   
  m_mrodId = 0; 
  m_runNum = 0; 
  m_runType = 0;     
  m_lvl1Id = 0;  
  m_ecrId = 0;     
  m_bcId = 0;  
  m_triggerTypeId = 0;
  m_DetEventType = 0;
}

void ALFA_RawDataContainerReadOut::setZeroFull()
{
  m_timestamp = 0;  
  m_BCId = 0;   
}

