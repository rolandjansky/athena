/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TightMuonElseNoFieldManager.h"
#include "CxxUtils/checker_macros.h"
#include "G4Track.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"

/// Normal constructor
TightMuonElseNoFieldManager::TightMuonElseNoFieldManager(G4Field *detectorField, G4ChordFinder *pChordFinder, G4bool b)
 : G4FieldManager(detectorField,pChordFinder,b)
 , m_globalField(nullptr)
{}

/// Simple constructor
TightMuonElseNoFieldManager::TightMuonElseNoFieldManager(G4MagneticField *detectorMagneticField)
 : G4FieldManager(detectorMagneticField) 
 , m_globalField(nullptr)
{}

/// The one interesting method
void TightMuonElseNoFieldManager::ConfigureForTrack(const G4Track * track)
{
  // If they have not been set yet, get the settings for the global field manager
  if (nullptr==m_globalField){
    // Ok, we're just holding it and giving it back to the base class,
    // and this method is not const.
    // But the G4 base class interface makes this impossible to avoid
    // (We could avoid it if G4FieldManager also had a non-const
    // GetDetectorField.)
    G4Field* field_nc ATLAS_THREAD_SAFE = const_cast<G4Field*>(GetDetectorField());
    m_globalField = field_nc;
  }

  // If this is a muon, move it in the magnetic field; otherwise turn off the stepping
  if (track->GetDefinition()==G4MuonPlus::Definition() ||
      track->GetDefinition()==G4MuonMinus::Definition() ){
    SetDetectorField(m_globalField);
  } else {
    SetDetectorField(nullptr);
  }

}
