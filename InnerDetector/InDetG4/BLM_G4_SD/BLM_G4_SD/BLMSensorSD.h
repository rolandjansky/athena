/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//###############################################
//   BLM Sensitive Detector class
//   Bostjan Macek 14.february.2008
//###############################################

#ifndef BLMSENSORSD_H
#define BLMSENSORSD_H

// Base class
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

// G4 needed classes
class G4Step;
class G4HCofThisEvent;

//StoreGate pointers FIXME... remove this
#include "SimHelpers/AthenaHitsCollectionHelper.h"

// use of the hits
#include "InDetSimEvent/SiHitCollection.h"

class BLMSensorSD : public FADS::FadsSensitiveDetector
{
 public:
  BLMSensorSD(G4String);
  ~BLMSensorSD();
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
 private:
  // The hits collection and the helper
  SiHitCollection *BLM_HitColl;
  AthenaHitsCollectionHelper m_hitCollHelp;

};

#endif
