/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArG4H6EmecMergerFactory.h"
#include "LArG4H6SD/LArHitsEventAction.h"
#include "LArG4H6SD/LArG4H6EmecMerger.h"

#include "StoreGate/StoreGateSvc.h"



LArG4H6EmecMergerFactory::LArG4H6EmecMergerFactory(StoreGateSvc* detStore,
						   LArHitsEventAction *action,
						   G4float timeBinWidth):
  m_detStore(detStore),
  m_action(action),
  m_timeBinWidth(timeBinWidth)
{
}

LArG4H6EmecMergerFactory::~LArG4H6EmecMergerFactory()
{
}

LArVHitMerger* LArG4H6EmecMergerFactory::getHitMerger() const
{
  return new LArG4H6EmecMerger(m_detStore,m_timeBinWidth);
}

 
