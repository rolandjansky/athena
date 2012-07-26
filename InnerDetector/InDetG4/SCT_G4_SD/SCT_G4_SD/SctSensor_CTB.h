/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   SCT Sensitive Detector class								 
   Davide Costanzo Jul 14th 2003.




 ****************************************************************/

#ifndef SCTSENSOR_CTB_H
#define SCTSENSOR_CTB_H

// Base class
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

// For messages
#include "AthenaKernel/MsgStreamMember.h"

// G4 needed classes
class G4Step;
class G4HCofThisEvent;

// For the hits
#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "InDetSimEvent/SiHitCollection.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class SctSensor_CTB : public FADS::FadsSensitiveDetector
{
 public:
  SctSensor_CTB(G4String);
  ~SctSensor_CTB();
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

 private:
  // The hits collection and the helper
  SiHitCollection *myHitColl;
  AthenaHitsCollectionHelper m_hitCollHelp;
  bool m_isGeoModel;
  mutable Athena::MsgStreamMember m_msg;
};

#endif




