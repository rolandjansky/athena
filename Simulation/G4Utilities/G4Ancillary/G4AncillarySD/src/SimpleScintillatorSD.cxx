/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AncillarySD/SimpleScintillatorSD.h"

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

// Geant4 Stuff
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"

// to permit access to StoreGate
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "MCTruth/TrackHelper.h"

#include "G4AncillarySimEvent/SimpleScintillatorHit.h"
#include "G4AncillarySimEvent/SimpleScintillatorHitCollection.h"

//////////////////////////////////////////////////////////////////////////////////
// Initialize static data
///////////////////////////////////////////////////////////////////////////////

static FADS::SensitiveDetectorEntryT<SimpleScintillatorSD> sd("SimpleScintillatorSD");

//______________________________________________________________________________
SimpleScintillatorSD::SimpleScintillatorSD(G4String name)
 : FadsSensitiveDetector(name) 
 , m_sgSvc(0)
 , hitCollection(0)
{
  AthenaHitsCollectionHelper m_hitCollHelp;

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode status = svcLocator->service("StoreGateSvc", m_sgSvc);
  if (status.isFailure()) {
    msg( MSG::FATAL ) << "StoreGateSvc not found!" << endreq;
  } else {
    msg( MSG::DEBUG ) << "StoreGateSvc initialized." << endreq;
  }

}


//______________________________________________________________________________
SimpleScintillatorSD::~SimpleScintillatorSD(){}


//______________________________________________________________________________
void SimpleScintillatorSD::Initialize(G4HCofThisEvent*)
{
  msg( MSG::VERBOSE ) << "Initializing SD for event" << endreq;
  hitCollection=new SimpleScintillatorHitCollection(GetName());

}

//_______________________________________________________________________________________________
G4bool SimpleScintillatorSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  msg( MSG::VERBOSE ) << "Process Hit. SD name is " << GetName() <<"; DetName is "<<GetDetectorName()<<endreq;

  G4StepPoint *preStep=aStep->GetPreStepPoint();
  G4StepPoint *postStep=aStep->GetPostStepPoint();
  G4VPhysicalVolume *preVol=preStep->GetPhysicalVolume();
  G4VPhysicalVolume *postVol=postStep->GetPhysicalVolume();
  if (preVol==postVol) return false;

  G4TouchableHistory*  myTouch = (G4TouchableHistory*)(preStep->GetTouchable());
  G4LogicalVolume* logVol __attribute__ ((unused)) = myTouch->GetVolume()->GetLogicalVolume();

  double edep = aStep->GetTotalEnergyDeposit();

  G4ThreeVector preStepPointPosition = preStep->GetPosition();

  double kineticEnergy = preStep->GetKineticEnergy();

  // Calculate the local step begin and end position.
  // From a G4 FAQ:
  // http://geant4-hn.slac.stanford.edu:5090/HyperNews/public/get/geometry/17/1.html
  const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();
  G4ThreeVector localPosition = transformation.TransformPoint(preStepPointPosition);

  G4Track * track = aStep->GetTrack();
  G4ParticleDefinition* pParticleDefinition = track->GetDefinition();
  int particleEncoding = pParticleDefinition->GetPDGEncoding();

  int side=1;
  if(preStepPointPosition.z()<0) side=-1;

  std::string volName= preVol->GetName();
  std::string volNumber=volName.substr(volName.size()-1,1);
  int copyNo =  myTouch->GetVolume()->GetCopyNo();
  char *endptr;
  int eta=strtol(volNumber.c_str(), &endptr, 0);
  if (endptr[0] != '\0') {
    msg( MSG::ERROR ) << "Could not convert string to int: " << volNumber << endreq;
    return false;
  }


  TrackHelper trHelp(track);
  int barcode = trHelp.GetBarcode();

  double globaltime = aStep->GetPreStepPoint()->GetGlobalTime();

  //Fill the hit:
  SimpleScintillatorHit * newHit = new SimpleScintillatorHit( particleEncoding,
							      kineticEnergy,
							      edep,
							      localPosition,
							      globaltime,
							      side*(eta*10+copyNo),
							      barcode);


  hitCollection->Insert(newHit);

  return true;
}


//_______________________________________________________________________________________________
void SimpleScintillatorSD::EndOfEvent(G4HCofThisEvent* )
{
  msg( MSG::DEBUG ) << "End of event. size of collection is  " << hitCollection->size()<<endreq;
  m_hitCollHelp.ExportCollection<SimpleScintillatorHitCollection>(hitCollection);

}


