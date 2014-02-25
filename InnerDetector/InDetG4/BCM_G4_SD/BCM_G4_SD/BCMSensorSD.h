/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//###############################################
//   BCM Sensitive Detector class
//   Bostjan Macek 17.may.2007
//###############################################

#ifndef BCMSENSORSD_H
#define BCMSENSORSD_H

// Base class
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

// G4 needed classes
class G4Step;
class G4HCofThisEvent;

#include "SimHelpers/AthenaHitsCollectionHelper.h"

// use of the hits
#include "InDetSimEvent/SiHitCollection.h"
#include "BCM_G4_SD/BCMExtra.h"

class BCMSensorSD : public FADS::FadsSensitiveDetector
{
 public:
  BCMSensorSD(G4String);
  ~BCMSensorSD();
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
 private:
  //
  AthenaHitsCollectionHelper m_hitCollHelp;

  // The hits collection
  SiHitCollection *BCM_HitColl;
};

#endif
