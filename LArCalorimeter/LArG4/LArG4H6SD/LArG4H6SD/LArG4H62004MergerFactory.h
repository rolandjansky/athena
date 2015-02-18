/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_LARG4H62004MERGERFACTORY_H
#define LARG4H6SD_LARG4H62004MERGERFACTORY_H

#include "LArG4Code/LArVHitMergerFactory.h"

class StoreGateSvc;
class LArHitsEventAction;
class LArVHitMerger;

class LArG4H62004MergerFactory : public LArVHitMergerFactory
{
 public:

  // Constructors and destructors.
  LArG4H62004MergerFactory(StoreGateSvc* detStore,
                      LArHitsEventAction* action,
                      G4float timeBinWidth);

  virtual ~LArG4H62004MergerFactory();

  LArVHitMerger* getHitMerger() const;

 private:
  StoreGateSvc*       m_detStore;
  G4float             m_timeBinWidth;

};

#endif // LARG4H6SD_LARG4H62004MERGERFACTORY_H
