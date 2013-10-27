/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class PhantomBarrelSD.
// Sensitive detector for the ancillary detectors
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// December 15th, 2003.
//
//************************************************************

#ifndef PhantomBarrelSD_H
#define PhantomBarrelSD_H 

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
using namespace FADS;
#include "TileSimEvent/TileHitVector.h"

class G4Step;
class G4HCofThisEvent;
class TileTBID;
class IMessageSvc;
class StoreGateSvc;
class PhantomBarrelSDMessenger;
class PhantomBarrelDescription;

class PhantomBarrelSD : public FadsSensitiveDetector
{
public:
  PhantomBarrelSD(G4String);
  ~PhantomBarrelSD();

  void Initialize(G4HCofThisEvent*);
  void Description(std::string);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

private:
  PhantomBarrelDescription* detectorDescription;
  IMessageSvc*  m_msgSvc;
  StoreGateSvc* m_sgSvc;
  StoreGateSvc* m_detStore;

  PhantomBarrelSDMessenger* phantombarrelSDMessenger;
  const TileTBID* m_tileTBID;
  bool  m_iamowner;

  static const int nCell = 8;

  int m_nhits[nCell];
  TileSimHit* m_hit[nCell];
  Identifier m_id[nCell];
};

#endif
