/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArCalibHitMergerFactory_H__
#define __LArCalibHitMergerFactory_H__

#include "LArG4Code/LArVCalibHitMergerFactory.h"
#include "LArG4HitManagement/LArCalibHitMerger.h"
#include <vector>

class StoreGateSvc;
class LArHitsEventAction;


class LArCalibHitMergerFactory : public LArVCalibHitMergerFactory
{
 public:

  // Constructors and destructors.
  LArCalibHitMergerFactory(StoreGateSvc* detStore,
			   LArHitsEventAction* action);

  virtual ~LArCalibHitMergerFactory();

  LArVCalibHitMerger* getHitMerger() const;

 private:
  StoreGateSvc*       m_detStore;
  LArHitsEventAction* m_action;

  // Keep pointers to all created hit mergers in order to avoid memory leaks
  typedef std::vector<LArCalibHitMerger* > CalibHitMergerVec;
  mutable CalibHitMergerVec m_hitMergers;
};

#endif // _LArCalibHitMergerFactory_H_
