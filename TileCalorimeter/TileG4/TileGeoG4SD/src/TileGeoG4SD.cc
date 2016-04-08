/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileGeoG4SD
// Sensitive detector for TileCal G4 simulations with TileGeoModel
//
// Author: Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
//
// Major updates: July, 2013 (Sergey)
//
//************************************************************

// class header
#include "TileGeoG4SD/TileGeoG4SD.hh"
// package headers
#include "TileGeoG4SD/TileGeoG4SDCalc.hh"
#include "TileGeoG4SD/TileGeoG4LookupBuilder.hh"
#include "TileGeoG4SD/TileGeoG4Lookup.hh"
// athena headers
#include "CxxUtils/make_unique.h"
// Geant4 headers
#include "G4Step.hh"

TileGeoG4SD::TileGeoG4SD(G4String name, const std::string& hitCollectionName, const TileSDOptions opts)
    : G4VSensitiveDetector(name),
    m_options(opts),
    m_HitColl(hitCollectionName)
{
  //Sensitive Detector initialisation for TileCal G4 simulations
  m_calc = new TileGeoG4SDCalc(m_options);

  //build tilecal ordinary look-up table
  m_lookup = m_calc->lookup;
  if (verboseLevel > 5)
    G4cout << "Lookup built for Tile" << G4endl;
}

TileGeoG4SD::~TileGeoG4SD() {
  delete m_calc;
  delete m_lookup;
}

void TileGeoG4SD::Initialize(G4HCofThisEvent* /*HCE*/) {
#ifdef ATHENAHIVE
  // Temporary fix for Hive until isValid is fixed
  m_HitColl = CxxUtils::make_unique<TileHitVector>(m_HitColl.name());
#else

  if (!m_HitColl.isValid())
    m_HitColl = CxxUtils::make_unique<TileHitVector>(m_HitColl.name());
#endif
}

G4bool TileGeoG4SD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/) {
  if (! (m_calc->FindTileScinSection(aStep))) { //Search for the tilecal sub-section, its module and some identifiers

    if (verboseLevel > 5)
      G4cout << "ProcessHits: FindTileScinSection(aStep) is false!" << G4endl;
    return false;
  }

  if ( !(m_calc->MakePmtEdepTime(aStep)) ) { //calculation of pmtID, edep and scin_Time with aStep (Sergey)

    if (verboseLevel > 10)
      G4cout << "ProcessHits: wrong pmtID_up,pmtID_down,edep_up,edep_down,"
             << "scin_Time_up,scin_Time_down:\t" << m_calc->pmtID_up
             << "\t" << m_calc->pmtID_down
             << "\t" << m_calc->edep_up
             << "\t" << m_calc->edep_down
             << "\t" << m_calc->scin_Time_up
             << "\t" << m_calc->scin_Time_down << G4endl;
    return false;
  }

  if ( !(m_calc->ManageScintHit()) ) { //create or update hit object in the collection

    G4cout << "ProcessHits: TileHit can not be produced; pmtID_up,pmtID_down,edep_up,edep_down,"
           << "scin_Time_up,scin_Time_down:\t" << m_calc->pmtID_up
           << "\t" << m_calc->pmtID_down
           << "\t" << m_calc->edep_up
           << "\t" << m_calc->edep_down
           << "\t" << m_calc->scin_Time_up
           << "\t" << m_calc->scin_Time_down << G4endl;
    return false;
  }

  return true;
}

void TileGeoG4SD::EndOfAthenaEvent() {
  m_lookup->ResetCells(&*m_HitColl);
}

