/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4HitManagement/LArHitMergerFactory.h"
#include "LArG4HitManagement/LArHitsEventAction.h"

#include "StoreGate/StoreGateSvc.h"



LArHitMergerFactory::LArHitMergerFactory(StoreGateSvc* detStore,
					 LArHitsEventAction* action,
					 std::string timeBinType,
					 G4float timeBinWidth):
  m_detStore(detStore),
  m_action(action),
  m_timeBinType(timeBinType),
  m_timeBinWidth(timeBinWidth)
{
}

LArHitMergerFactory::~LArHitMergerFactory()
{
  // Clean up the vector of hit mergers
  HitMergerVec::iterator first = m_hitMergers.begin();
  HitMergerVec::iterator last = m_hitMergers.end();
  for(; first!=last; first++)
    delete (*first);
}

LArVHitMerger* LArHitMergerFactory::getHitMerger() const
{
  LArHitMerger* hitMerger = new LArHitMerger(m_detStore,m_action,m_timeBinType,m_timeBinWidth);
  m_hitMergers.push_back(hitMerger);
  return hitMerger;
}

