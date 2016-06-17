/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TightMuonElseNoFieldManager.h"
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
    m_globalField = const_cast<G4Field*>(GetDetectorField());
  }

  // If this is a muon, move it in the magnetic field; otherwise turn off the stepping
  if (track->GetDefinition()==G4MuonPlus::Definition() ||
      track->GetDefinition()==G4MuonMinus::Definition() ){
    SetDetectorField(m_globalField);
  } else {
    SetDetectorField(nullptr);
  }

}
