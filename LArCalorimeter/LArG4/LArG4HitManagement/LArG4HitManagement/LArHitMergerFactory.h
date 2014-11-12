/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArHitMergerFactory_H__
#define __LArHitMergerFactory_H__

#include "LArG4HitManagement/LArHitMerger.h"
#include "LArG4Code/LArVHitMergerFactory.h"
#include <string>
#include <vector>

class StoreGateSvc;
class LArHitsEventAction;

class LArHitMergerFactory : public LArVHitMergerFactory
{
 public:

  // Constructors and destructors.
  LArHitMergerFactory(StoreGateSvc* detStore,
		      LArHitsEventAction* action,
		      std::string timeBinType,
		      G4float timeBinWidth);

  virtual ~LArHitMergerFactory();

  LArVHitMerger* getHitMerger() const;

 private:
  StoreGateSvc*       m_detStore;
  LArHitsEventAction* m_action;
  std::string         m_timeBinType;
  G4float             m_timeBinWidth;

  // Keep pointers to all created hit mergers in order to avoid memory leaks
  typedef std::vector<LArHitMerger* > HitMergerVec;
  mutable HitMergerVec m_hitMergers;
};

#endif // _LArHitMergerFactory_H_
