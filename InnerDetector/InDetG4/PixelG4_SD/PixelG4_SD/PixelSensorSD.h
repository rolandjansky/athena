/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Pixel Sensitive Detector class								 
   Davide Costanzo Sep 4th 2002.




 ****************************************************************/

#ifndef PIXELSENSORSD_H
#define PIXELSENSORSD_H

// Base class
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

// For messages
#include "AthenaKernel/MsgStreamMember.h"

// G4 needed classes
class G4Step;
class G4HCofThisEvent;

// StoreGate pointers FIXME... remove this
class AthenaHitsCollectionHelper;
// #include "SimHelpers/AthenaHitsCollectionHelper.h"

// use of the hits
#include "InDetSimEvent/SiHitCollection.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class PixelSensorSD : public FADS::FadsSensitiveDetector
{
 public:
  PixelSensorSD(G4String);
  ~PixelSensorSD();
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
  MsgStream& msg(MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLvl(MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

 private:
  // The hits collection and the helper
  SiHitCollection *myHitColl;
  AthenaHitsCollectionHelper m_hitCollHelp;
  bool m_isGeoModel;
  mutable Athena::MsgStreamMember m_msg;
};

#endif




