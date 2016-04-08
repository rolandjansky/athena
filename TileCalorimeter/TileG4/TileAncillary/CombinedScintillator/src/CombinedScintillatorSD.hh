/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class CombinedScintillatorSD
// Sensitive detector for the Scintillator between LAR & Tile
//
//************************************************************

#ifndef CombinedScintillator_CombinedScintillatorSD_H
#define CombinedScintillator_CombinedScintillatorSD_H

// Base Class
#include "G4VSensitiveDetector.hh"

// use of the hits
#include "TileSimEvent/TileHitVector.h"
#include "StoreGate/WriteHandle.h"

class G4Step;
class G4HCofThisEvent;
class TileTBID;
class CombinedScintillatorDescription;

class CombinedScintillatorSD : public G4VSensitiveDetector
{
public:
  CombinedScintillatorSD(const std::string& name, const std::string& hitCollectionName);
  ~CombinedScintillatorSD();

  void StartOfAthenaEvent();   // Called from CombinedScintillatorSDTool::SetupEvent
  void Initialize(G4HCofThisEvent*) override final;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;
  void EndOfAthenaEvent();

private:
  const TileTBID* m_tileTBID;

  static const int nCell = 2;

  int m_nhits[nCell];
  TileSimHit* m_hit[nCell];
  Identifier m_id[nCell];
  // The hits collections
  SG::WriteHandle<TileHitVector> m_HitColl;
};

#endif
