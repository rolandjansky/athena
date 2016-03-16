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
#include "ISF_Event/EntryLayer.h"

#include "ISF_Interfaces/IParticleBroker.h"

// Athena includes
#include "AtlasDetDescr/AtlasRegion.h"

#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

#include "MCTruth/EventInformation.h"
#include "MCTruth/PrimaryParticleInformation.h"
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
//#include "G4VPhysicalVolume.hh"

#include <iostream>

iGeant4::TrackProcessorUserActionBase::TrackProcessorUserActionBase(const std::string& name)
  : ITrackProcessorUserAction(name),
    m_eventInfo(nullptr),
    m_curBaseISP(nullptr)
    //TODO ELLI , AthMessaging( msgSvc(), name)
{

  //TODO ELLI ATH_MSG_DEBUG("create TrackProcessorUserActionBase name: "<<name);

}

void iGeant4::TrackProcessorUserActionBase::BeginOfRunAction(const G4Run*)
{
  // FIXME: the m_volumeOffset and m_minHistoryDepth variables don't seem to be used anywhere
  //
  //// get the geometry manager and check how many layers are present.
  //G4TransportationManager *transportationManager(G4TransportationManager::GetTransportationManager());
  //G4LogicalVolume *world((*(transportationManager->GetWorldsIterator()))->GetLogicalVolume());
  //ATH_MSG_VERBOSE("World G4LogicalVolume Name: " << world->GetName() << " has " << world->GetNoDaughters() << " daughters.");
  //if ("World::World"==world->GetName())
  //  {
  //    ATH_MSG_INFO("Atlas::Atlas is not the world volume, so assume we are in a cosmics job.");
  //    //Cosmics-specific configuration.
  //    m_volumeOffset=2;
  //    m_minHistoryDepth=1;
  //  }
}

void iGeant4::TrackProcessorUserActionBase::EndOfRunAction(const G4Run*)
{

}

void iGeant4::TrackProcessorUserActionBase::BeginOfEventAction(const G4Event*)
{
  m_curBaseISP = nullptr;
  m_eventInfo  = ISFG4Helpers::getEventInformation();
}

void iGeant4::TrackProcessorUserActionBase::EndOfEventAction(const G4Event*)
{
  m_curBaseISP = nullptr;
  m_eventInfo  = nullptr;
}

void iGeant4::TrackProcessorUserActionBase::SteppingAction(const G4Step* aStep)
{
  // get geoID from parent
  //TODO ELLI AtlasDetDescr::AtlasRegion curGeoID = m_curBaseISP->nextGeoID();
  //TODO ELLI ATH_MSG_DEBUG( "Currently simulating TrackID = " << aStep->GetTrack()->GetTrackID() <<
  //TODO ELLI                " inside geoID = " << curGeoID );

  //
  // call the ISFSteppingAction method of the implementation
  // 
  ISFSteppingAction( aStep, m_curBaseISP );

  //
  // propagate the current ISFParticle link to all secondaries
  //
  const std::vector<const G4Track*>  *secondaryVector = aStep->GetSecondaryInCurrentStep();
  for ( auto* aConstSecondaryTrack : *secondaryVector ) {
    // get a non-const G4Track for current secondary (nasty!)
    G4Track* aSecondaryTrack = const_cast<G4Track*>( aConstSecondaryTrack );

    auto *trackInfo = ISFG4Helpers::getISFTrackInfo(*aSecondaryTrack);

    // G4Tracks aready returned to ISF will have a TrackInformation attached to them
    bool particleReturnedToISF = trackInfo && trackInfo->GetReturnedToISF();
    if (!particleReturnedToISF) {
      HepMC::GenParticle* generationZeroTruthParticle = nullptr;
      ISFG4Helpers::attachTrackInfoToNewG4Track( *aSecondaryTrack,
                                                 *m_curBaseISP,
                                                 Secondary,
                                                 generationZeroTruthParticle );
    }
  } // <- loop over secondaries from this step

  return;
}

void iGeant4::TrackProcessorUserActionBase::PostUserTrackingAction(const G4Track* /*aTrack*/)
{
  m_curBaseISP = nullptr;

  return;
}

void iGeant4::TrackProcessorUserActionBase::PreUserTrackingAction(const G4Track* aTrack)
{
  // what a great way to start a function... :)
  G4Track* inT = const_cast<G4Track*> (aTrack);

  auto *trackInfo = ISFG4Helpers::getISFTrackInfo(*aTrack);

  // will be filled later on
  HepMC::GenParticle *currentlyTracedHepPart = nullptr;


  //
  // Get PrimaryParticleInformation from G4PrimaryParticle (assigned by TransportTool::addPrimaryVertex)
  // The ISFParticle should always exist, and the HepMC::GenParticle should exist if a primary EvGen particle (secondaries passed from G4 back to ISF for subsequent processing with G4 will have null pointer for HepMC::GenParticle)
  //

  // see if primary particle
  int   parentID = aTrack->GetParentID();
  bool isPrimary = !parentID;

  if (isPrimary) {
    if ( trackInfo ) {
      G4ExceptionDescription description;
      description << G4String("PreUserTrackingAction: ")
                  << "Started simulating primary particle which already has a TrackInformation/TrackBarcodeInfo object attached (trackID: "
                  << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                  << ", parentID " << aTrack->GetParentID() << ")";
      G4Exception("iGeant4::TrackProcessorUserActionBase", "TrackInformationAlreadyExists", FatalException, description);
      return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }

    auto ppInfo = dynamic_cast <PrimaryParticleInformation*> (aTrack->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation());
    if (!ppInfo) {
        G4ExceptionDescription description;
        description << G4String("PreUserTrackingAction: ") + "NULL PrimaryParticleInformation pointer for current G4Step (trackID "
                    << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                    << ", parentID " << parentID << ")";
        G4Exception("iGeant4::TrackProcessorUserActionBase", "NoPPInfo", FatalException, description);
        return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }

    // get base ISFParticle and link to TrackInformation
    auto baseISP = ppInfo->GetISFParticle();
    if (!baseISP) {
        G4ExceptionDescription description;
        description << G4String("PreUserTrackingAction: ") + "No ISFParticle associated with primary particle (trackID: "
                    << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                    << ", parentID " << parentID << ")";
        G4Exception("iGeant4::TrackProcessorUserActionBase", "NoISFParticle", FatalException, description);
        return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }

    auto truthBinding = baseISP->getTruthBinding();
    if (!truthBinding) {
        G4ExceptionDescription description;
        description << G4String("PreUserTrackingAction: ") + "No ISF::TruthBinding associated with primary particle (trackID: "
                    << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                    << ", parentID " << parentID << ")";
        G4Exception("iGeant4::TrackProcessorUserActionBase", "NoISFTruthBinding", FatalException, description);
        return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }

    int   regenerationNr              = ppInfo->GetRegenerationNr();
    auto* primaryTruthParticle        = truthBinding->getGenerationZeroTruthParticle();
    auto* generationZeroTruthParticle = truthBinding->getGenerationZeroTruthParticle();
          currentlyTracedHepPart      = truthBinding->getTruthParticle();

    TrackClassification classification = Secondary;
    // if particle points to a non-zero truth particle it can not just be a 'simple' Secondary
    if (currentlyTracedHepPart) {
      if      (currentlyTracedHepPart==primaryTruthParticle)                          { classification = Primary; }
      else if (generationZeroTruthParticle==primaryTruthParticle && regenerationNr>0) { classification = RegeneratedPrimary; }
      else                                                                            { classification = RegisteredSecondary; }
    }

    auto* newTrackInfo = ISFG4Helpers::attachTrackInfoToNewG4Track( *inT,
                                                                    *baseISP,
                                                                    classification,
                                                                    generationZeroTruthParticle );
    newTrackInfo->SetRegenerationNr(regenerationNr);
    trackInfo = newTrackInfo;
  } else {
    // secondary particle (why does TrackInformation return a const object!?)
    currentlyTracedHepPart = const_cast<HepMC::GenParticle*>( trackInfo->GetHepMCParticle() );
  }

  // why does TrackInformation return a const object!?
  auto primaryTruthParticle = const_cast<HepMC::GenParticle*>( trackInfo->GetPrimaryHepMCParticle() );
  m_eventInfo->SetCurrentlyTraced( currentlyTracedHepPart );
  m_eventInfo->SetCurrentPrimary ( primaryTruthParticle   );

  // get link to base ISFParticle from G4Track user information
  m_curBaseISP = const_cast<ISF::ISFParticle*>( trackInfo->GetBaseISFParticle() );

  return;
}

