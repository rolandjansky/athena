/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


const RpcSectorLogicContainer::SectorListElement_t*
RpcSectorLogicContainer::sectorList() const
{
  return m_sectorList;
}


void RpcSectorLogicContainer::setSectorList (const SectorList_t& sectorList)
{
  m_sectorList[0] = sectorList[0];
  m_sectorList[1] = sectorList[1];
}
