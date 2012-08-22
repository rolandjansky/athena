/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "EventContainers/SelectAllObject.h"
#include <map>
#include <cassert>
#include <iostream>

using std::ostream;
using std::endl;

//**********************************************************************
// Local definitions
//**********************************************************************

//**********************************************************************
// Member functions.
//**********************************************************************


// Default constructor.

ALFA_RawDataContainer::ALFA_RawDataContainer():
  //: IdentifiableContainer<ALFA_RawDataCollection>() 
  DataVector<ALFA_RawDataCollection>(),
  m_wordMarker(0),
  m_dataWord(0),
  m_subdetId(0),
  m_mrodId(0),
  m_lvl1Id(0),
  m_ecrId(0),
  m_bcId(0),
  m_runNum(0),
  m_runType(0),
  m_triggerTypeId(0),
  m_DetEventType(0),
  TimeStamp(0),
  TimeStampns(0),
  BCId(0),
  LumiBlock(0),
  lvl1Pattern()
{
}


//**********************************************************************

// Destructor.

ALFA_RawDataContainer::~ALFA_RawDataContainer() {

}

//**********************************************************************


// Return the total number of digits in the container.

ALFA_RawDataContainer::size_type ALFA_RawDataContainer::digit_size() const {
  ALFA_RawDataContainer::size_type count = 0;
  ALFA_RawDataContainer::const_iterator it = begin();
  ALFA_RawDataContainer::const_iterator iend = end();

  for (; it != iend; ++it ) {
    count += (*it)->size();
  }
  return count;
}


void ALFA_RawDataContainer::SetTimeStamp(uint32_t tmpTimeStamp)
{
  TimeStamp=tmpTimeStamp;
}

void ALFA_RawDataContainer::SetTimeStampns(uint32_t tmpTimeStampns)
{
  TimeStampns=tmpTimeStampns;
}

void ALFA_RawDataContainer::SetLumiBlock(uint32_t tmpLumiBlock)
{
  LumiBlock=tmpLumiBlock;
}

void ALFA_RawDataContainer::SetBCId(uint32_t tmpBCId)
{
  BCId=tmpBCId;
}

void ALFA_RawDataContainer::SetLvl1Pattern(std::vector<bool> tmplvl1_pattern)
{
  lvl1Pattern = tmplvl1_pattern;
}

void ALFA_RawDataContainer::SetLvl2Pattern(std::vector<bool> tmplvl2_pattern)
{
  lvl2Pattern = tmplvl2_pattern;
}

void ALFA_RawDataContainer::SetEFPattern(std::vector<bool> tmpef_pattern)
{
  efPattern = tmpef_pattern;
}



void ALFA_RawDataContainer::setZero()
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

  TimeStamp=0; 
  TimeStampns=0;
  LumiBlock=0;
  BCId=0;
  lvl1Pattern.clear();
  lvl2Pattern.clear();
  efPattern.clear();

}

