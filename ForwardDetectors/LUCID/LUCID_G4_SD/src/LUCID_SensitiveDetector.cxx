/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VUserPrimaryParticleInformation.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

#include "LUCID_G4_SD/LUCID_SensitiveDetector.h"
#include "LUCID_GeoModel/LUCID_Constants.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

static FADS::SensitiveDetectorEntryT<LUCID_SensitiveDetector> sd("LUCID_SensitiveDetector");

LUCID_SensitiveDetector::LUCID_SensitiveDetector(std::string name): FADS::FadsSensitiveDetector(name) {  

  m_hit = new LUCID_HitHelper();

  m_lucidSimHitCollection = 0;
}

LUCID_SensitiveDetector::~LUCID_SensitiveDetector()  {}

void LUCID_SensitiveDetector::Initialize(G4HCofThisEvent* /*HCE*/) {
  
  ATH_MSG_DEBUG ( " LUCID_SensitiveDetector::Initialize " );
  
  m_lucidSimHitCollection = new LUCID_SimHitCollection("LucidSimHitsVector");
}

bool LUCID_SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  
  ATH_MSG_DEBUG ( " LUCID_SensitiveDetector::ProcessHits " );
    
  G4Track* aTrack = aStep->GetTrack();  
  
  if (aTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  ATH_MSG_DEBUG ( " There is an OpticalPhoton " );

  aTrack->SetTrackStatus(fKillTrackAndSecondaries);
  
  if (aTrack->GetCreatorProcess()->GetProcessName() != "Cerenkov") return false;

  ATH_MSG_DEBUG ( " It is from a Cerenkov process " );
  
  double energy = aTrack->GetKineticEnergy()/CLHEP::eV;
  double lambda = m_hit->GetWaveLength(energy);

  LUCID_SimHit* aHit = new LUCID_SimHit(m_hit->GetTubNumber(aStep),
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
  
  m_lucidSimHitCollection->Insert(*aHit);
  
  return true;
}

void LUCID_SensitiveDetector::BeginOfEvent(G4HCofThisEvent*) {}

void LUCID_SensitiveDetector::EndOfEvent(G4HCofThisEvent*) {  

  ATH_MSG_DEBUG ( " LUCID_SensitiveDetector::EndOfEvent " );

  m_hitCollHelp.ExportCollection<LUCID_SimHitCollection>(m_lucidSimHitCollection);
}
