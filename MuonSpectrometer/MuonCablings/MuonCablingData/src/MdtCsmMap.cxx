/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MdtCsmMap.h"
#include "MuonCablingData/MdtAmtMap.h"


// default constructor
MdtCsmMap::MdtCsmMap(uint8_t csmId):
  MdtMapBase<MdtAmtMap>(csmId,"MdtAmtMap")
{
  if (m_debug) { 
    *m_log << MSG::VERBOSE << "Adding Csm number: " << (int) csmId << endmsg;
  }
}


// destructor
MdtCsmMap::~MdtCsmMap()
{ }

// add a TDC, with its Id, to the CSM
bool MdtCsmMap::setAmtMap(uint8_t tdcId, MdtAmtMap* amtMap) {
  bool tdcAdded = addItem(tdcId,amtMap);
  return tdcAdded;
}

// retrieve the TDC corresponding to a given online Id
MdtAmtMap* MdtCsmMap::getTdcMap(uint8_t tdcId)
{
  return getItem(tdcId);
}
