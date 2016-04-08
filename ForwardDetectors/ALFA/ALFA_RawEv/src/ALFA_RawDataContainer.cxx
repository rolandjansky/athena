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
  //m_dataWord(0),
  m_subdetId(0),
  m_mrodId(0),
  m_lvl1Id(0),
  m_ecrId(0),
  m_bcId(0),
  m_runNum(0),
  m_runType(0),
  m_triggerTypeId(0),
  m_DetEventType(0),
  m_TimeStamp(0),
  m_TimeStampns(0),
  m_BCId(0),
  m_LumiBlock(0),
  m_lvl1Pattern()
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
  m_TimeStamp=tmpTimeStamp;
}

void ALFA_RawDataContainer::SetTimeStampns(uint32_t tmpTimeStampns)
{
  m_TimeStampns=tmpTimeStampns;
}

void ALFA_RawDataContainer::SetLumiBlock(uint32_t tmpLumiBlock)
{
  m_LumiBlock=tmpLumiBlock;
}

void ALFA_RawDataContainer::SetBCId(uint32_t tmpBCId)
{
  m_BCId=tmpBCId;
}

void ALFA_RawDataContainer::SetLvl1Pattern(const std::vector<bool>& tmplvl1_pattern)
{
  m_lvl1Pattern = tmplvl1_pattern;
}

void ALFA_RawDataContainer::SetLvl2Pattern(const std::vector<bool>& tmplvl2_pattern)
{
  m_lvl2Pattern = tmplvl2_pattern;
}

void ALFA_RawDataContainer::SetEFPattern(const std::vector<bool>& tmpef_pattern)
{
  m_efPattern = tmpef_pattern;
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

  m_TimeStamp=0; 
  m_TimeStampns=0;
  m_LumiBlock=0;
  m_BCId=0;
  m_lvl1Pattern.clear();
  m_lvl2Pattern.clear();
  m_efPattern.clear();

}

