/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackProcessorUserActionBase.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "TrackProcessorUserActionBase.h"

// local package includes
#include "ISFG4Helpers.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ITruthBinding.h"
#include "ISF_Event/EntryLayer.h"

#include "ISF_HepMC_Event/HepMC_TruthBinding.h"

#include "ISF_Interfaces/IParticleBroker.h"

// Athena includes
#include "AtlasDetDescr/AtlasRegion.h"

#include "MCTruth/EventInformation.h"
#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/VTrackInformation.h"

#include "SimHelpers/StepHelper.h"
#include "StoreGate/StoreGateSvc.h"

// Geant4 includes
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
#include "G4LogicalVolumeStore.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"

//#include "G4VPhysicalVolume.hh"

#include <iostream>

iGeant4::TrackProcessorUserActionBase::TrackProcessorUserActionBase(const std::string& type, const std::string& name, const IInterface* parent)
  : UserActionBase(type,name,parent),
    m_curTrackID(-999),
    m_curISP(0)
{
    //  declareInterface<IUserAction>(this);
}

void iGeant4::TrackProcessorUserActionBase::BeginOfEvent(const G4Event*)
{
  //std::cout<<"clearing ISFParticle map"<<std::endl;
  m_curISP     =    0;
  m_curTrackID = -999;
}

void iGeant4::TrackProcessorUserActionBase::Step(const G4Step* aStep)
{
  G4Track*           aTrack   = aStep->GetTrack();
  int               aTrackID  = aTrack->GetTrackID();

  // update m_curISP if TrackID has changed or current ISP is unset
  if ( aTrackID!=m_curTrackID || !m_curISP ){
    m_curTrackID = aTrackID;

    // get link to last ISFParticle from G4Track user information
    G4VUserTrackInformation *g4TrackInfo = aTrack->GetUserInformation();
    VTrackInformation       *trackInfo   = dynamic_cast<VTrackInformation*>( g4TrackInfo );
    const ISF::ISFParticle  *attachedISFP = (trackInfo) ? trackInfo->GetISFParticle() : 0;
    m_curISP = const_cast<ISF::ISFParticle*>( attachedISFP );

    if ( !m_curISP ) {
      //ELLI ATH_MSG_ERROR("No corresponding ISFParticle link found for current G4Track (TrackID=="<< m_curTrackID << ") !");
      G4ExceptionDescription description;
      description << G4String("SteppingAction: ") + "NULL ISFParticle pointer for current G4Step (trackID "
                  << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                  << ", parentID " << aTrack->GetParentID() << ")";
      G4Exception("iGeant4::TrackProcessorUserActionPassBack", "NoISFParticle", FatalException, description);
      return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }
  }

  // get geoID from parent
  //ELLI AtlasDetDescr::AtlasRegion curGeoID = m_curISP->nextGeoID();
  //ELLI ATH_MSG_DEBUG( "Currently simulating TrackID = " << m_curTrackID <<
  //ELLI                " inside geoID = " << curGeoID );

  //
  // call the ISFSteppingAction method of the implementation
  // 
  ISFSteppingAction( aStep, m_curISP );

  //
  // propagate the current ISFParticle link to all secondaries
  //
  const std::vector<const G4Track*> *secondaryVector = aStep->GetSecondaryInCurrentStep();
  for ( const G4Track* aConstSecondaryTrack : *secondaryVector ) {
    // get a non-const G4Track for current secondary (nasty!)
    G4Track *aSecondaryTrack = const_cast<G4Track*>( aConstSecondaryTrack );

    ISFG4Helpers::linkG4TrackToISFParticle( *aSecondaryTrack, m_curISP );
  }

  return;
}

void iGeant4::TrackProcessorUserActionBase::PreTracking(const G4Track* aTrack)
{
  G4Track* inT = const_cast<G4Track*> (aTrack);
  TrackHelper trackHelper(inT);

  EventInformation* eventInfo = static_cast<EventInformation*>
    (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());

  if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary()) {
    HepMC::GenParticle* part=const_cast<HepMC::GenParticle*>(trackHelper.GetTrackInformation()->
                                                             GetHepMCParticle());

    if (trackHelper.IsPrimary()) eventInfo->SetCurrentPrimary(part);

    eventInfo->SetCurrentlyTraced(part);
  }

  // see if primary particle
  int pID = aTrack->GetParentID();
  bool isPrimary = !pID;

  if (!isPrimary) return;

  //
  // Get PrimaryParticleInformation from G4PrimaryParticle (assigned by TransportTool::addPrimaryVertex)
  // The ISFParticle should always exist, and the HepMC::GenParticle should exist if a primary EvGen particle (secondaries passed from G4 back to ISF for subsequent processing with G4 will have null pointer for HepMC::GenParticle)
  //

  PrimaryParticleInformation* ppInfo = dynamic_cast <PrimaryParticleInformation*> (aTrack->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation());
  if (!ppInfo) {
      G4ExceptionDescription description;
      description << G4String("PreUserTrackingAction: ") + "NULL PrimaryParticleInformation pointer for current G4Step (trackID "
                  << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                  << ", parentID " << aTrack->GetParentID() << ")";
      G4Exception("iGeant4::TrackProcessorUserActionBase", "NoPPInfo", FatalException, description);
      return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }
  // get parent
  const ISF::ISFParticle *parent = ppInfo->GetISFParticle();

  //ELLI if (!parent) {ATH_MSG_ERROR("no ISFParticle in PrimaryParticleInformation for "<<aTrack->GetTrackID()<<"!");}

  // Set the event info properly
  eventInfo->SetCurrentISFPrimary(parent);

  // store the link to the ISFParticle inside the G4Track
  G4Track *aTrackNonConst = const_cast<G4Track*>( aTrack );
  ISFG4Helpers::linkG4TrackToISFParticle( *aTrackNonConst, parent );

  return;
}


StatusCode iGeant4::TrackProcessorUserActionBase::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
