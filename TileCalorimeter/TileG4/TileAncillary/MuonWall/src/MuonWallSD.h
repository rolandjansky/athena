/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MuonWallSD
// Sensitive detector for the Muon Wall
//
//************************************************************

#ifndef MUONWALL_MUONWALLSD_H
#define MUONWALL_MUONWALLSD_H

// Base class
#include "G4VSensitiveDetector.hh"

// use of the hits
#include "TileSimEvent/TileHitVector.h"
#include "StoreGate/WriteHandle.h"

// STL header
#include <string>

class G4Step;
class G4TouchableHistory;

class G4Step;
class G4HCofThisEvent;
class TileTBID;

class MuonWallSD : public G4VSensitiveDetector
{
public:
  MuonWallSD(const std::string& name, const std::string& hitCollectionName, int verbose);
  ~MuonWallSD();

  // Called from MuonWallSDTool::SetupEvent
  void StartOfAthenaEvent ();
  void Initialize(G4HCofThisEvent*) override final;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;
  void EndOfAthenaEvent();

private:
  const TileTBID* m_tileTBID;

  static const int s_nCellMu = 14;
  static const int s_nCellS = 4;
  static const int s_nCell = s_nCellMu+s_nCellS;

  int m_nhits[s_nCell];
  TileSimHit* m_hit[s_nCell];
  Identifier m_id[s_nCell];
  // The hits collections
  SG::WriteHandle<TileHitVector> m_HitColl;

};

#endif // MUONWALL_MUONWALLSD_H
