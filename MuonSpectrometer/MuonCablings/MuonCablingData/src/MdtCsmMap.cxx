/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MdtCsmMap.h"
#include "MuonCablingData/MdtAmtMap.h"


// default constructor
MdtCsmMap::MdtCsmMap(uint8_t csmId):
  MdtMapBase<MdtAmtMap>(csmId,"MdtAmtMap")
{

}


// add a TDC, with its Id, to the CSM
bool MdtCsmMap::setAmtMap(uint8_t tdcId, MdtAmtMap* amtMap, MsgStream &log) {
  bool tdcAdded = addItem(tdcId,amtMap, log);
  return tdcAdded;
}

// retrieve the TDC corresponding to a given online Id
MdtAmtMap* MdtCsmMap::getTdcMap(uint8_t tdcId)
{
  return getItem(tdcId);
}
