/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gTowerContainer - Container for the gFEX towers
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/gTowerContainer.h"
#include "L1CaloFEXSim/gTower.h"
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Geometry/Vector3D.h"
#include <boost/algorithm/cxx11/partition_point.hpp>
#include <atomic>

namespace LVL1{

gTowerContainer::gTowerContainer(SG::OwnershipPolicy ownPolicy) :
  DataVector<LVL1::gTower>(ownPolicy)
{
  m_map_towerID_containerIndex.clear();
}

void gTowerContainer::push_back(int ieta, int iphi, int nphi, int keybase, int posneg)
{
  DataVector<LVL1::gTower>::push_back(std::make_unique<gTower>(ieta,iphi,nphi,keybase,posneg));
}

void gTowerContainer::print() const {
  REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "gTowerContainer") << "gTowerContainer::print not implemented";
}


const LVL1::gTower * gTowerContainer::findTower(int towerID) const
{

  const auto it = m_map_towerID_containerIndex.find(towerID);

  if (it == m_map_towerID_containerIndex.end()) {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "gTowerContainer") << "Requested tower ID "
                                                                  << towerID
                                                                  << " not found in container.";
    return nullptr;
  }

  const int container_index = it->second;

  if (container_index < 0) {
    return nullptr;
  }

  return (*this)[container_index];
}


LVL1::gTower * gTowerContainer::findTower(int towerID)
{

  const auto it = m_map_towerID_containerIndex.find(towerID);

  if (it == m_map_towerID_containerIndex.end()) {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "gTowerContainer") << "Requested tower ID "
                                                                  << towerID
                                                                  << " not found in container.";
    return nullptr;
  }

  const int container_index = it->second;

  if (container_index < 0) {
    return nullptr;
  }

  return (*this)[container_index];
}

void gTowerContainer::clearContainerMap()
{
  m_map_towerID_containerIndex.clear();
}

bool gTowerContainer::fillContainerMap(){
  clearContainerMap();
  size_t ntowers = size();
  for (size_t itower = 0; itower < ntowers; itower++) {
    const gTower * theTower = (*this)[itower];
    int towerID = theTower->constid();
    int container_index = itower;
    m_map_towerID_containerIndex.insert(std::pair<int,int>(towerID,container_index));
  }
  return true;
}

}
