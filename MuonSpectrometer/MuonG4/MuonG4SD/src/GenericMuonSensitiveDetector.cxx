/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonG4SD/GenericMuonSensitiveDetector.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "MCTruth/TrackHelper.h"

#include "G4Track.hh"

#include <string>
#include <iostream>
#include <limits>

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// Initialize static data
static FADS::SensitiveDetectorEntryT<GenericMuonSensitiveDetector> gensd("GenericMuonSensitiveDetector");



// construction/destruction
GenericMuonSensitiveDetector::GenericMuonSensitiveDetector(std::string name)
  : FadsSensitiveDetector(name), m_GenericMuonHitCollection(0)
{
  std::cout<< " creating a GenericMuonSensitiveDetector: "<<name<<std::endl;
}


// Implemenation of memebr functions
void GenericMuonSensitiveDetector::Initialize(G4HCofThisEvent*) 
{
  //m_GenericMuonHitCollection=new GenericMuonSimHitCollection("GenericMuonSensitiveDetector");
  m_GenericMuonHitCollection = m_hitCollHelp.RetrieveNonconstCollection<GenericMuonSimHitCollection>("GenericMuonSensitiveDetector");
}

G4bool GenericMuonSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* /*ROHist*/) 
{
  std::cout << "\t\t GenericMuonSensitiveDetector: Hit in a sensitive layer!!!!! "<<std::endl;
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

  GenericMuonSimHit* aHit=new GenericMuonSimHit(0 /* HitID id generic*/,globalTime,globalpreTime,position,local_position,preposition,local_preposition,pdgCode,eKin,direction,depositEnergy,StepLength,barcode);

  std::cout<<aHit->print()<<std::endl;
  m_GenericMuonHitCollection->Insert(*aHit);
  delete aHit;

  return true;
}


void GenericMuonSensitiveDetector::EndOfEvent(G4HCofThisEvent* /*HCE*/) 
{
  if(!m_allowMods)
    m_hitCollHelp.SetConstCollection<GenericMuonSimHitCollection>(m_GenericMuonHitCollection);
} 


