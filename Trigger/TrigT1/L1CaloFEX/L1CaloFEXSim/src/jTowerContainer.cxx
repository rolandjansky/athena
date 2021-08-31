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

namespace LVL1 {

jTowerContainer::jTowerContainer(SG::OwnershipPolicy ownPolicy) :
    DataVector<LVL1::jTower>(ownPolicy)
{
    m_map_towerID_containerIndex.clear();
}

void jTowerContainer::push_back(float eta, float phi, int key_eta, float keybase, int posneg, float centre_eta, float centre_phi, int fcal_layer)
{
    DataVector<LVL1::jTower>::push_back(std::make_unique<jTower>(eta,phi,key_eta,keybase,posneg,centre_eta,centre_phi,fcal_layer));
}

void jTowerContainer::print() const {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "jTowerContainer") << "jTowerContainer::print not implemented";
}


const LVL1::jTower * jTowerContainer::findTower(int towerID) const
{
    const auto it = m_map_towerID_containerIndex.find(towerID);
    if (it==m_map_towerID_containerIndex.cend()) {
        // FIXME !!!
        // This should be:
        // return nullptr;
        // but the previous version of the code had a bug and relied on UNDEFINED BEHAVIOUR
        // which would often set container_index to 0 for out-of-range towerID, but maybe
        // sometimes to a different number. I keep the container_index=0 for now while only
        // refactoring the code for performance. Rafal Bielski 31/08/2021
        return (*this)[0];
    }
    const int container_index = it->second;
    if (container_index < 0) {
        return nullptr;
    }
    return (*this)[container_index];
}

LVL1::jTower * jTowerContainer::findTower(int towerID)
{
    const auto it = m_map_towerID_containerIndex.find(towerID);
    if (it==m_map_towerID_containerIndex.cend()) {
        // FIXME !!!
        // This should be:
        // return nullptr;
        // but the previous version of the code had a bug and relied on UNDEFINED BEHAVIOUR
        // which would often set container_index to 0 for out-of-range towerID, but maybe
        // sometimes to a different number. I keep the container_index=0 for now while only
        // refactoring the code for performance. Rafal Bielski 31/08/2021
        return (*this)[0];
    }
    const int container_index = it->second;
    if (container_index < 0) {
        return nullptr;
    }
    return (*this)[container_index];
}

void jTowerContainer::clearContainerMap()
{
    m_map_towerID_containerIndex.clear();
}

bool jTowerContainer::fillContainerMap() {
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
