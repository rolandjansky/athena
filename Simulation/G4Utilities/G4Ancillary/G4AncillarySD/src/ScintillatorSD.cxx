/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AncillarySD/ScintillatorSD.h"
#include "G4AncillarySD/ScintillatorSD_XMLHandler.h"
#include "G4AncillarySD/ScintillatorSD_PyHandler.h"

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
#include "CLHEP/Units/PhysicalConstants.h"

#include "G4AncillarySimEvent/ScintillatorHit.h"
#include "G4AncillarySimEvent/ScintillatorHitCollection.h"

//////////////////////////////////////////////////////////////////////////////////
// Initialize static data
///////////////////////////////////////////////////////////////////////////////

static FADS::SensitiveDetectorEntryT<ScintillatorSD> sd("ScintillatorSD");

bool ScintillatorSD::sm_sourceIsDelayed = false;
bool ScintillatorSD::sm_createdBefore = false;
std::vector<G4LogicalVolume *> ScintillatorSD::sm_delayed_logVols;
std::vector<std::string> ScintillatorSD::sm_delayed_hitCollectionNames;
std::vector<bool> ScintillatorSD::sm_delayed_writeHitsWithZeroEnergies;

//______________________________________________________________________________
ScintillatorSD::ScintillatorSD(G4String name): FadsSensitiveDetector(name) {

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode status = svcLocator->service("StoreGateSvc", m_sgSvc);
  if (status.isFailure()) {
    msg( MSG::FATAL ) << "StoreGateSvc not found!" << endreq;
  } else {
    msg( MSG::DEBUG ) << "StoreGateSvc initialized." << endreq;
  }

  if (sm_createdBefore) {
    msg( MSG::FATAL ) << "I am not the first instance of this class. This won't work!!" << endreq;
  } else {
    sm_createdBefore = true;

    m_volToHCMap.clear();
    m_HCMap.clear();
    m_storeHitsWithZeroEnergy.clear();

    if (sm_sourceIsDelayed) {
      //add the volumes in the static lists
      for (unsigned int i = 0; i < sm_delayed_logVols.size(); ++i) {
		   addVolume( sm_delayed_logVols.at(i),
		   sm_delayed_hitCollectionNames.at(i),
		   sm_delayed_writeHitsWithZeroEnergies.at(i) );
      }
      sm_delayed_logVols.clear();
      sm_delayed_hitCollectionNames.clear();
      sm_delayed_writeHitsWithZeroEnergies.clear();
    } else {
      //Since we didn't get any data filled in earlier, we will start
      //the XML handler, hoping that it will take care of business.
      ScintillatorSD_XMLHandler * dummy __attribute__ ((unused)) = new ScintillatorSD_XMLHandler("ScintillatorParameters",this);
    };
  };
}


//______________________________________________________________________________
ScintillatorSD::~ScintillatorSD(){}

//______________________________________________________________________________
void ScintillatorSD::addVolumeDelayed( G4LogicalVolume * logVol,
				       std::string hitCollectionName,
				       bool writeHitsWithZeroEnergy ) {
  sm_sourceIsDelayed = true;
  sm_delayed_logVols.push_back(logVol);
  sm_delayed_hitCollectionNames.push_back(hitCollectionName);
  sm_delayed_writeHitsWithZeroEnergies.push_back(writeHitsWithZeroEnergy);
  return;
}


//______________________________________________________________________________
void ScintillatorSD::addVolume( G4LogicalVolume * logVol , std::string hitCollectionName, bool writeHitsWithZeroEnergy ) {

  // Find out if a hit collection with that name has already been made
  //otherwise make it:
  if ( m_HCMap.find(hitCollectionName) == m_HCMap.end() ) {
    m_HCMap[hitCollectionName] = NULL;
  }

  // Check that the volume does not already have a HC associated and
  //make the association:
  if ( m_volToHCMap.find(logVol) == m_volToHCMap.end() ) {
    m_volToHCMap[logVol] = &(m_HCMap[hitCollectionName]);
    if (msgLvl(MSG::DEBUG)) msg( MSG::DEBUG ) <<"addVolume: Added volume " << logVol << " and HC "
	<< hitCollectionName <<". Currently "<<m_HCMap.size()<< " are registered" << endreq;
  } else {
    msg( MSG::ERROR ) << "addVolume: Logical Volume " << logVol
	<< " already has associated hit collection!" << endreq;
  };

  //Check that the volume does not already have a switch for writing hits with zero energy, and set it:
  if ( m_storeHitsWithZeroEnergy.find(logVol) == m_storeHitsWithZeroEnergy.end() ) {
    m_storeHitsWithZeroEnergy[logVol] = writeHitsWithZeroEnergy;
  } else {
    msg( MSG::ERROR ) << "addVolume: Logical Volume " << logVol
	<< "already has switch for storing hits with zero energy!" << endreq;
  };

}

//______________________________________________________________________________
void ScintillatorSD::Initialize(G4HCofThisEvent* )
{
  msg( MSG::VERBOSE ) << "Initializing SD for event" << endreq;

  for ( HCMapIter hc = m_HCMap.begin(); hc != m_HCMap.end(); hc++) {
    msg( MSG::VERBOSE ) << "Making new HC for : " << hc->first << endreq;
    hc->second = new ScintillatorHitCollection(hc->first);
  };

}

//_______________________________________________________________________________________________
G4bool ScintillatorSD::ProcessHits(G4Step* aStep,G4TouchableHistory* )
{
  msg( MSG::VERBOSE ) << "Process Hit" << endreq;

  G4TouchableHistory*  myTouch = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  G4LogicalVolume* logVol = myTouch->GetVolume()->GetLogicalVolume();

  double edep = aStep->GetTotalEnergyDeposit();
  if( edep==0. && !m_storeHitsWithZeroEnergy[logVol] ) return false;


  G4ThreeVector preStepPointPosition = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector postStepPointPosition = aStep->GetPostStepPoint()->GetPosition();

  double kineticEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();

  // Calculate the local step begin and end position.
  // From a G4 FAQ:
  // http://geant4-hn.slac.stanford.edu:5090/HyperNews/public/get/geometry/17/1.html
  const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();
  G4ThreeVector localPreStep = transformation.TransformPoint(preStepPointPosition);
  G4ThreeVector localPostStep = transformation.TransformPoint(postStepPointPosition);

  G4Track * track = aStep->GetTrack();
  G4ParticleDefinition* pParticleDefinition = track->GetDefinition();
  int particleEncoding = pParticleDefinition->GetPDGEncoding();

  int copyNo =  myTouch->GetVolume()->GetCopyNo();

  TrackHelper trHelp(track);
  int barcode = trHelp.GetBarcode();

  double globaltime = aStep->GetPreStepPoint()->GetGlobalTime();

  //Fill the hit:
  ScintillatorHit * newHit = new ScintillatorHit( copyNo,
						  particleEncoding,
						  kineticEnergy,
						  edep,
						  localPreStep.x(),
						  localPreStep.y(),
						  localPreStep.z(),
						  localPostStep.x(),
						  localPostStep.y(),
						  localPostStep.z(),
						  preStepPointPosition.x(),
						  preStepPointPosition.y(),
						  preStepPointPosition.z(),
						  postStepPointPosition.x(),
						  postStepPointPosition.y(),
						  postStepPointPosition.z(),
						  globaltime,
						  barcode);

  // Figure out which HC to use:
  if ( m_volToHCMap.find(logVol) != m_volToHCMap.end() ) {
    (*(m_volToHCMap[logVol]))->Insert( newHit );
    if (msgLvl(MSG::VERBOSE)) msg( MSG::VERBOSE ) <<"Hit in ScintillatorSD : Volume "<<logVol
        <<", copyNo = "<< copyNo
        <<", particleEncoding = = "<< particleEncoding
        <<", kineticEnergy = "<< kineticEnergy/CLHEP::GeV << " GeV"
        <<", edep = "<< edep/CLHEP::keV<< " keV"
        <<", localPreStep.x() = "<< localPreStep.x()/CLHEP::cm<<" cm"
        <<", localPreStep.y() = "<< localPreStep.y()/CLHEP::cm<<" cm"
        <<", localPreStep.z() = "<< localPreStep.z()/CLHEP::cm<<" cm"
        <<", localPostStep.x() = "<< localPostStep.x()/CLHEP::cm<<" cm"
        <<", localPostStep.y() = "<< localPostStep.y()/CLHEP::cm<<" cm"
        <<", localPostStep.z() = "<< localPostStep.z()/CLHEP::cm<<" cm"
        <<", preStepPointPosition.x() = "<< preStepPointPosition.x()/CLHEP::cm<<" cm"
        <<", preStepPointPosition.y() = "<< preStepPointPosition.y()/CLHEP::cm<<" cm"
        <<", preStepPointPosition.z() = "<< preStepPointPosition.z()/CLHEP::cm<<" cm"
        <<", postStepPointPosition.x() = "<< postStepPointPosition.x()/CLHEP::cm<<" cm"
        <<", postStepPointPosition.y() = "<< postStepPointPosition.y()/CLHEP::cm<<" cm"
        <<", postStepPointPosition.z() = "<< postStepPointPosition.z()/CLHEP::cm<<" cm"
        <<", globaltime = "<< globaltime/CLHEP::nanosecond<<" ns"
        <<", barcode = "<< barcode<<" "<<endreq;
  } else {
    msg( MSG::ERROR ) << "Process Hit::Logical Volume not found in list! Please add volume name to xml file" << endreq;
  };


  return true;
}


//_______________________________________________________________________________________________
void ScintillatorSD::EndOfEvent(G4HCofThisEvent*)
{
  msg( MSG::DEBUG ) << "End of event " << endreq;

  for ( HCMapConstIter hc = m_HCMap.begin(); hc != m_HCMap.end(); hc++) {
    msg( MSG::INFO ) << "Exporting ScintillatorHitCollection : " << hc->first << endreq;
    m_hitCollHelp.ExportCollection<ScintillatorHitCollection>(hc->second);
  };

}


