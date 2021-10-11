/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MdtRODMap.h"
#include "MuonCablingData/MdtCsmMap.h"


// constructor
MdtRODMap::MdtRODMap(uint8_t rodId) :
  MdtMapBase<MdtCsmMap>(rodId,"MdtCsmMap")
{ }



// add a CSM map to this ROD
bool MdtRODMap::setCsmMap(uint8_t csmId, MdtCsmMap* csmMap, MsgStream &log)
{ 
  bool csmAdded = addItem(csmId,csmMap, log);
  if (!csmAdded) {
    log << MSG::ERROR << "Could not add csm " << MSG::hex 
	   << (int) csmId << MSG::dec << " to the MDT cabling map" << endmsg;
  }
  return csmAdded;
}


// retrieve the CSM corresponding to a given online Id
MdtCsmMap* MdtRODMap::getCsmMap(uint8_t csmId)
{
  return getItem(csmId);
}
