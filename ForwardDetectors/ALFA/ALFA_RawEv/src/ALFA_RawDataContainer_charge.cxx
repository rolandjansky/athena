/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "ALFA_RawEv/ALFA_RawDataContainer_charge.h"
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

ALFA_RawDataContainer_charge::ALFA_RawDataContainer_charge():
  //: IdentifiableContainer<ALFA_RawDataCollection_charge>() 
  DataVector<ALFA_RawDataCollection_charge>(),
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
  m_DetEventType(0)
{
}


//**********************************************************************

// Destructor.

ALFA_RawDataContainer_charge::~ALFA_RawDataContainer_charge() {

}

//**********************************************************************


// Return the total number of digits in the container.

  ALFA_RawDataContainer_charge::size_type ALFA_RawDataContainer_charge::digit_size() const {
  ALFA_RawDataContainer_charge::size_type count = 0;
  ALFA_RawDataContainer_charge::const_iterator it = begin();
  ALFA_RawDataContainer_charge::const_iterator iend = end();

  for (; it != iend; ++it ) {
    count += (*it)->size();
  }
  return count;
}

void ALFA_RawDataContainer_charge::setZero()
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

