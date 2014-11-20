/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class CombinedScintillatorSD
// Sensitive detector for the Scintillator between LAR & Tile
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************

#ifndef CombinedScintillatorSD_H
#define CombinedScintillatorSD_H 

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
using namespace FADS;
#include "TileSimEvent/TileHitVector.h"

class G4Step;
class G4HCofThisEvent;
class TileTBID;
class IMessageSvc;
class StoreGateSvc;
class CombinedScintillatorSDMessenger;
class CombinedScintillatorDescription;

class CombinedScintillatorSD : public FadsSensitiveDetector
{
public:
  CombinedScintillatorSD(G4String);
  ~CombinedScintillatorSD();

  void Initialize(G4HCofThisEvent*);
  void Description(std::string);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

private:
  CombinedScintillatorDescription* detectorDescription;
  IMessageSvc*  m_msgSvc;
  StoreGateSvc* m_sgSvc;
  StoreGateSvc* m_detStore;

  CombinedScintillatorSDMessenger* combinedscintillatorSDMessenger;
  const TileTBID* m_tileTBID;
  bool  m_iamowner;

  static const int nCell = 2;

  int m_nhits[nCell];
  TileSimHit* m_hit[nCell];
  Identifier m_id[nCell];
};

#endif
