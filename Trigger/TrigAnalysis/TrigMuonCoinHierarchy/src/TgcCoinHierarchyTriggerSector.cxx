/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/TgcCoinHierarchyTriggerSector.h"
#include "TrigMuonCoinHierarchy/TgcCoinHierarchy.h"

using Muon::TgcCoinData;
using Muon::TgcPrepData;

namespace Trigger {
  TgcCoinHierarchyTriggerSector::TgcCoinHierarchyTriggerSector() 
    : m_bc(TgcCoinHierarchy::UNKNOWN) 
  {
  }

  unsigned int TgcCoinHierarchyTriggerSector::addCoincidence(const TgcCoinData* coin, 
                                                             const TgcCoinHierarchy::COINCIDENCE coinType) {
    m_coins[coinType][m_bc].push_back(coin); 
    m_coinsAssociated[coinType][m_bc].push_back(false); 

    // Return id of the coincidence
    return m_coins[coinType][m_bc].size()-1; 
  }

  unsigned int TgcCoinHierarchyTriggerSector::addHit(const TgcPrepData* hit, 
                                                     const TgcCoinHierarchy::TYPE hitType, 
                                                     const TgcCoinHierarchy::STATION station) {
    m_hits[hitType][station][m_bc].push_back(hit);
    m_hitsAssociated[hitType][station][m_bc].push_back(false);

    // Return id of the hit
    return m_hits[hitType][station][m_bc].size()-1;
  }

  const TgcCoinData* TgcCoinHierarchyTriggerSector::getCoincidence(const unsigned int id, 
                                                                   const TgcCoinHierarchy::COINCIDENCE coinType) const {
    return m_coins[coinType][m_bc].at(id);
  }

  const TgcPrepData* TgcCoinHierarchyTriggerSector::getHit(const unsigned int id, 
                                                           const TgcCoinHierarchy::TYPE hitType, 
                                                           const TgcCoinHierarchy::STATION station) const {
    return m_hits[hitType][station][m_bc].at(id);
  }

  unsigned int TgcCoinHierarchyTriggerSector::numCoincidences(const TgcCoinHierarchy::COINCIDENCE coinType) const {
    return m_coins[coinType][m_bc].size();
  }

  unsigned int TgcCoinHierarchyTriggerSector::numHits(const TgcCoinHierarchy::TYPE hitType, 
                                                      const TgcCoinHierarchy::STATION station) const {
    return m_hits[hitType][station][m_bc].size();
  }

  void TgcCoinHierarchyTriggerSector::setCoincidenceAssociated(const unsigned int id, 
                                                               const TgcCoinHierarchy::COINCIDENCE coinType) {
    m_coinsAssociated[coinType][m_bc].at(id) = true;
  }

  void TgcCoinHierarchyTriggerSector::setHitAssociated(const unsigned int id, 
                                                       const TgcCoinHierarchy::TYPE hitType, 
                                                       const TgcCoinHierarchy::STATION station) {
    m_hitsAssociated[hitType][station][m_bc].at(id) = true;
  }

  bool TgcCoinHierarchyTriggerSector::isCoincidenceAssociated(const unsigned int id, 
                                                              const TgcCoinHierarchy::COINCIDENCE coinType) const {
    return m_coinsAssociated[coinType][m_bc].at(id);
  }

  bool TgcCoinHierarchyTriggerSector::isHitAssociated(const unsigned int id, 
                                                      const TgcCoinHierarchy::TYPE hitType, 
                                                      const TgcCoinHierarchy::STATION station) const {
    return m_hitsAssociated[hitType][station][m_bc].at(id);
  }

  TgcCoinHierarchy::TIMING TgcCoinHierarchyTriggerSector::setTiming(const TgcCoinHierarchy::TIMING indexBcid){
    m_bc = indexBcid;
    return m_bc;
  }

  TgcCoinHierarchy::TIMING TgcCoinHierarchyTriggerSector::getTiming()const{
    return m_bc;
  }

} // end of namespace Trigger
