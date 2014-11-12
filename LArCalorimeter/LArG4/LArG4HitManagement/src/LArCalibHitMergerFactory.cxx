/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4HitManagement/LArCalibHitMergerFactory.h"
#include "LArG4HitManagement/LArHitsEventAction.h"

#include "StoreGate/StoreGateSvc.h"


LArCalibHitMergerFactory::LArCalibHitMergerFactory(StoreGateSvc* detStore,
						   LArHitsEventAction* action):
  m_detStore(detStore),
  m_action(action)
{
}

LArCalibHitMergerFactory::~LArCalibHitMergerFactory()
{
  // Clean up the vector of hit mergers
  CalibHitMergerVec::iterator first = m_hitMergers.begin();
  CalibHitMergerVec::iterator last = m_hitMergers.end();
  for(; first!=last; first++)
    delete (*first);
}

LArVCalibHitMerger* LArCalibHitMergerFactory::getHitMerger() const
{
  LArCalibHitMerger* hitMerger = new LArCalibHitMerger(m_detStore,m_action);
  m_hitMergers.push_back(hitMerger);
  return hitMerger;
}

