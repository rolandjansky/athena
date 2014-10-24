/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H6EmecMergerFactory_H__
#define __LArG4H6EmecMergerFactory_H__

#include "LArG4Code/LArVHitMergerFactory.h"

class StoreGateSvc;
class LArHitsEventAction;
class LArVHitMerger;

class LArG4H6EmecMergerFactory : public LArVHitMergerFactory
{
 public:

  // Constructors and destructors.
  LArG4H6EmecMergerFactory(StoreGateSvc* detStore,
                      LArHitsEventAction* action,
                      G4float timeBinWidth);

  virtual ~LArG4H6EmecMergerFactory();

  LArVHitMerger* getHitMerger() const;

 private:
  StoreGateSvc*       m_detStore;
  LArHitsEventAction* m_action;
  G4float             m_timeBinWidth;

};

#endif // _LArG4H6EmecMergerFactory_H_
