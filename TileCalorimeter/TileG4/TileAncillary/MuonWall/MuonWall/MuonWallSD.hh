/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MuonWallSD
// Sensitive detector for the Muon Wall
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************

#ifndef MuonWallSD_H
#define MuonWallSD_H 

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
using namespace FADS;
#include "TileSimEvent/TileHitVector.h"

class G4Step;
class G4HCofThisEvent;
class TileTBID;
class IMessageSvc;
class StoreGateSvc;
class MuonWallSDMessenger;
class MuonWallDescription;

class MuonWallSD : public FadsSensitiveDetector
{
public:
  MuonWallSD(G4String);
  ~MuonWallSD();

  void Initialize(G4HCofThisEvent*);
  void Description(std::string);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

private:
  MuonWallDescription* detectorDescription;
  IMessageSvc*  m_msgSvc;
  StoreGateSvc* m_sgSvc;
  StoreGateSvc* m_detStore;

  MuonWallSDMessenger* muonwallSDMessenger;
  const TileTBID* m_tileTBID;
  bool  m_iamowner;

  static const int nCell = 14;

  int m_nhits[nCell];
  TileSimHit* m_hit[nCell];
  Identifier m_id[nCell];
};

#endif
