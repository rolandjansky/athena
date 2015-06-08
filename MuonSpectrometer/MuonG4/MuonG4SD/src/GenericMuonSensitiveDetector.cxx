/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GenericMuonSensitiveDetector.h"
#include "MCTruth/TrackHelper.h"
#include "CxxUtils/make_unique.h" // For make unique

#include "G4Track.hh"

#include <string>

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// construction/destruction
GenericMuonSensitiveDetector::GenericMuonSensitiveDetector(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_GenericMuonHitCollection( hitCollectionName )
{
  G4cout << " creating a GenericMuonSensitiveDetector: "<<name << G4endl;
}

// Implemenation of memebr functions
void GenericMuonSensitiveDetector::Initialize(G4HCofThisEvent*) 
{
  if (!m_GenericMuonHitCollection.isValid()) m_GenericMuonHitCollection = CxxUtils::make_unique<GenericMuonSimHitCollection>();
}

G4bool GenericMuonSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* /*ROHist*/) 
{
  G4cout << "Hit in a sensitive layer!!!!! " << G4endl;
  G4Track* currentTrack = aStep->GetTrack();
  const G4AffineTransform trans = currentTrack->GetTouchable()->GetHistory()->GetTopTransform(); // from global to local
  G4StepPoint* postStep=aStep->GetPostStepPoint();
  G4StepPoint* preStep=aStep->GetPreStepPoint();
  const G4Step* post_Step=aStep->GetTrack()->GetStep();

  Amg::Vector3D position = Amg::Hep3VectorToEigen( postStep->GetPosition() );
  Amg::Vector3D preposition = Amg::Hep3VectorToEigen( preStep->GetPosition() );
  
  Amg::Vector3D local_position = Amg::Hep3VectorToEigen( trans.TransformPoint( postStep->GetPosition() ) ) ;
  Amg::Vector3D local_preposition = Amg::Hep3VectorToEigen( trans.TransformPoint( preStep->GetPosition() ) );
  
  int pdgCode=currentTrack->GetDefinition()->GetPDGEncoding();

  float globalTime=postStep->GetGlobalTime();
  float globalpreTime=preStep->GetGlobalTime();
  float eKin=postStep->GetKineticEnergy();

  Amg::Vector3D direction = Amg::Hep3VectorToEigen( postStep->GetMomentumDirection() );
  float depositEnergy=post_Step->GetTotalEnergyDeposit();
  float StepLength=post_Step->GetStepLength();

  TrackHelper trHelp(aStep->GetTrack());
  int barcode = trHelp.GetBarcode();

  //G4cout << aHit->print() << G4endl;
  m_GenericMuonHitCollection->Emplace( 0 /* HitID id generic*/,globalTime,globalpreTime,position,local_position,preposition,local_preposition,pdgCode,eKin,direction,depositEnergy,StepLength,barcode);

  return true;
}

