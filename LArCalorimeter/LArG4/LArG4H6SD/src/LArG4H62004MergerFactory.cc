/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArG4H62004MergerFactory.h"
#include "LArG4H6SD/LArHitsEventAction.h"
#include "LArG4H6SD/LArG4H62004Merger.h"

#include "StoreGate/StoreGateSvc.h"



LArG4H62004MergerFactory::LArG4H62004MergerFactory(StoreGateSvc* detStore,
					 LArHitsEventAction* action,
					 G4float timeBinWidth):
  m_detStore(detStore),
  m_action(action),
  m_timeBinWidth(timeBinWidth)
{
}

LArG4H62004MergerFactory::~LArG4H62004MergerFactory()
{
}

LArVHitMerger* LArG4H62004MergerFactory::getHitMerger() const
{
  return new LArG4H62004Merger(m_detStore,m_timeBinWidth);
}

