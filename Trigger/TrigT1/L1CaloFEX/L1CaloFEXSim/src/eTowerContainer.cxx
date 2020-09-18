/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Code stolen shamelessly from Calorimeter/CaloEvent/src/CaloCellContainer.cxx and modified

#include "L1CaloFEXSim/eTowerContainer.h"
#include "L1CaloFEXSim/eTower.h"
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Geometry/Vector3D.h"
#include <boost/algorithm/cxx11/partition_point.hpp>
#include <atomic>

namespace LVL1{

eTowerContainer::eTowerContainer(SG::OwnershipPolicy ownPolicy) : 
  DataVector<LVL1::eTower>(ownPolicy)
{ 
  m_map_towerID_containerIndex.clear();
}

void eTowerContainer::push_back(float eta, float phi, float keybase, int posneg)
{
  DataVector<LVL1::eTower>::push_back(std::make_unique<eTower>(eta,phi,keybase,posneg));
}

void eTowerContainer::print() const {
  REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "eTowerContainer") << "eTowerContainer::print not implemented";
}


const LVL1::eTower * eTowerContainer::findTower(int towerID) const
{
  int container_index = -1;
  container_index = m_map_towerID_containerIndex.find(towerID)->second;
  if(container_index >= 0){
    return (*this)[container_index];
  }
  return 0;
}

LVL1::eTower * eTowerContainer::findTower(int towerID)
{
  int container_index = -1;
  container_index = m_map_towerID_containerIndex.find(towerID)->second;
  if(container_index >= 0){
    return (*this)[container_index];
  }
  return 0;
}

void eTowerContainer::clearContainerMap()
{
  m_map_towerID_containerIndex.clear();
}

bool eTowerContainer::fillContainerMap(){
  clearContainerMap();
  size_t ntowers = size();
  for (size_t itower = 0; itower < ntowers; itower++) {
    const eTower * theTower = (*this)[itower];
    int towerID = theTower->constid();
    int container_index = itower;
    m_map_towerID_containerIndex.insert(std::pair<int,int>(towerID,container_index));
  }
  return true;
}

}
