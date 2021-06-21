/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MdtSubdetectorMap.h"
#include "MuonCablingData/MdtRODMap.h"

MdtSubdetectorMap::MdtSubdetectorMap(uint8_t subdetectorId) :
  MdtMapBase<MdtRODMap>(subdetectorId,"MdtRODMap")
{ }

MdtSubdetectorMap::~MdtSubdetectorMap() 
{ }

//
// add a ROD to the cabling map
bool MdtSubdetectorMap::setRODMap(uint8_t rodId, MdtRODMap* mdtRODMap, MsgStream &log)
{
  bool rodAdded = addItem(rodId,mdtRODMap, log);
  return rodAdded;
}

// retrieve the ROD corresponding to a given online Id
MdtRODMap* MdtSubdetectorMap::getRODMap(uint8_t rodId)
{
  return getItem(rodId);
}
