/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "LUCID_SensitiveDetector.h"

// Package headers
#include "LUCID_HitHelper.h"

// Athena headers
#include "CxxUtils/make_unique.h" // For make unique
#include "LUCID_GeoModel/LUCID_Constants.h"

// Geant4 headers
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4OpticalPhoton.hh"

// CLHEP headers
#include "CLHEP/Units/PhysicalConstants.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LUCID_SensitiveDetector::LUCID_SensitiveDetector(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_HitColl( hitCollectionName )
{
  m_hit = new LUCID_HitHelper();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Initialize from G4 - necessary to new the write handle for now
void LUCID_SensitiveDetector::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<LUCID_SimHitCollection>(m_HitColl.name());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool LUCID_SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*) {

  if (verboseLevel>5)
    {
      G4cout << "LUCID_SensitiveDetector::ProcessHits - Begin" << G4endl;
    }
  G4Track* aTrack = aStep->GetTrack();

  if (aTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  if (verboseLevel>5)
    {
      G4cout << "LUCID_SensitiveDetector::ProcessHits(): There is an OpticalPhoton " << G4endl;
    }

  aTrack->SetTrackStatus(fKillTrackAndSecondaries);

  if (aTrack->GetCreatorProcess()->GetProcessName() != "Cerenkov") return false;

  if (verboseLevel>5)
    {
      G4cout << "LUCID_SensitiveDetector::ProcessHits(): It is from a Cerenkov process "  << G4endl;
    }

  double energy = aTrack->GetKineticEnergy()/CLHEP::eV;
  double lambda = m_hit->GetWaveLength(energy);

  m_HitColl->Emplace(m_hit->GetTubNumber(aStep),
                     aTrack->GetTrackID(),
                     aTrack->GetDefinition()->GetPDGEncoding(),
                     m_hit->GetVolNumber    (aTrack->GetLogicalVolumeAtVertex()->GetName()),
                     m_hit->GetPreStepPoint (aStep).x(),
                     m_hit->GetPreStepPoint (aStep).y(),
                     m_hit->GetPreStepPoint (aStep).z(),
                     m_hit->GetPostStepPoint(aStep).x(),
                     m_hit->GetPostStepPoint(aStep).y(),
                     m_hit->GetPostStepPoint(aStep).z(),
                     m_hit->GetPreStepTime  (aStep),
                     m_hit->GetPostStepTime (aStep),
                     lambda,
                     energy);
  return true;
}
