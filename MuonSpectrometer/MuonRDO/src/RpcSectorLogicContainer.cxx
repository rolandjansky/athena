/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcSectorLogicContainer.h"

RpcSectorLogicContainer::RpcSectorLogicContainer() :
  DataVector<RpcSectorLogic>()
{
  for (int i=0 ; i<2 ; ++i) m_sectorList[i]=0;
}

bool RpcSectorLogicContainer::findSector(const uint16_t sectorId, uint16_t side) const {
  uint32_t sector = (1<<(sectorId-side*32));
  return ( m_sectorList[side] & sector);
}

bool RpcSectorLogicContainer::setSector(uint16_t sectorId, uint16_t side) {
  uint32_t sector = (1<<(sectorId-side*32));
  if ( !(m_sectorList[side] & sector)) {
    m_sectorList[side]=m_sectorList[side]|sector;
    return true;
  }
  else {
    return false;
  }
}


