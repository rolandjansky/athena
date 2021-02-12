/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Code stolen shamelessly from Calorimeter/CaloEvent/src/CaloCellContainer.cxx and modified

#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/jTower.h"
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Geometry/Vector3D.h"
#include <boost/algorithm/cxx11/partition_point.hpp>
#include <atomic>

namespace LVL1{

jTowerContainer::jTowerContainer(SG::OwnershipPolicy ownPolicy) : 
  DataVector<LVL1::jTower>(ownPolicy)
{ 
  m_map_towerID_containerIndex.clear();
}

  void jTowerContainer::push_back(float eta, float phi, float keybase, int posneg, float centre_eta, float centre_phi, int fcal_layer)
{
  DataVector<LVL1::jTower>::push_back(std::make_unique<jTower>(eta,phi,keybase,posneg,centre_eta,centre_phi,fcal_layer));
}

void jTowerContainer::print() const {
  REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "jTowerContainer") << "jTowerContainer::print not implemented";
}


const LVL1::jTower * jTowerContainer::findTower(int towerID) const
{
  int container_index = -1;
  container_index = m_map_towerID_containerIndex.find(towerID)->second;
  if(container_index >= 0){
    return (*this)[container_index];
  }
  return nullptr;
}

LVL1::jTower * jTowerContainer::findTower(int towerID)
{
  int container_index = -1;
  container_index = m_map_towerID_containerIndex.find(towerID)->second;
  if(container_index >= 0){
    return (*this)[container_index];
  }
  return nullptr;
}

void jTowerContainer::clearContainerMap()
{
  m_map_towerID_containerIndex.clear();
}

bool jTowerContainer::fillContainerMap(){
  clearContainerMap();
  size_t ntowers = size();
  for (size_t itower = 0; itower < ntowers; itower++) {
    const jTower * theTower = (*this)[itower];
    int towerID = theTower->constid();
    int container_index = itower;
    m_map_towerID_containerIndex.insert(std::pair<int,int>(towerID,container_index));
  }
  return true;
}

}
