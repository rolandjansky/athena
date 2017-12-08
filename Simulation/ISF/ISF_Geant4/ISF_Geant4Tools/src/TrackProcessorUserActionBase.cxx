/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackProcessorUserActionBase.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "TrackProcessorUserActionBase.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/EntryLayer.h"

#include "ISF_Interfaces/IParticleBroker.h"

// ISF Geant4 includes
#include "ISF_Geant4Event/ISFG4Helper.h"

// Athena includes
#include "AtlasDetDescr/AtlasRegion.h"

#include "MCTruth/EventInformation.h"
#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackInformation.h"

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

namespace G4UA {

namespace iGeant4 {

TrackProcessorUserActionBase::TrackProcessorUserActionBase():
  m_eventInfo(nullptr),
  m_curBaseISP(nullptr)
{;
}

void TrackProcessorUserActionBase::BeginOfEventAction(const G4Event*)
{
  m_curBaseISP = nullptr;
  m_eventInfo = ::iGeant4::ISFG4Helper::getEventInformation();
  return;
}

void TrackProcessorUserActionBase::EndOfEventAction(const G4Event*)
{
  m_curBaseISP = nullptr;
  m_eventInfo = nullptr;
  return;
}

void TrackProcessorUserActionBase::UserSteppingAction(const G4Step* aStep)
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

    auto *trackInfo = ::iGeant4::ISFG4Helper::getISFTrackInfo(*aSecondaryTrack);

    // G4Tracks aready returned to ISF will have a TrackInformation attached to them
    bool particleReturnedToISF = trackInfo && trackInfo->GetReturnedToISF();
    if (!particleReturnedToISF) {
      HepMC::GenParticle* generationZeroTruthParticle = nullptr;
      ::iGeant4::ISFG4Helper::attachTrackInfoToNewG4Track( *aSecondaryTrack,
                                                *m_curBaseISP,
                                                Secondary,
                                                generationZeroTruthParticle );
    }
  } // <- loop over secondaries from this step

  return;
}

void TrackProcessorUserActionBase::PreUserTrackingAction(const G4Track* aTrack)
{
  bool isPrimary = ! aTrack->GetParentID();
  if (isPrimary) {
    G4Track* nonConstTrack = const_cast<G4Track*> (aTrack); // love it :)
    setupPrimary(*nonConstTrack);
  } else {
    setupSecondary(*aTrack);
  }

  return;
}

void TrackProcessorUserActionBase::setupPrimary(G4Track& aTrack)
{
  //
  // Get PrimaryParticleInformation from G4PrimaryParticle (assigned by TransportTool::addPrimaryVertex)
  //

  auto* trackInfo = ::iGeant4::ISFG4Helper::getISFTrackInfo(aTrack);
  if ( trackInfo ) {
    G4ExceptionDescription description;
    description << G4String("PreUserTrackingAction: ")
                << "Started simulation of primary particle which already has a TrackInformation/TrackBarcodeInfo object attached (trackID: "
                << aTrack.GetTrackID() << ", track pos: "<<aTrack.GetPosition() << ", mom: "<<aTrack.GetMomentum()
                << ", parentID " << aTrack.GetParentID() << ")";
    G4Exception("iGeant4::TrackProcessorUserActionBase", "TrackInformationAlreadyExists", FatalException, description);
    return; // The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }

  auto* ppInfo = dynamic_cast <PrimaryParticleInformation*> (aTrack.GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation());
  if (!ppInfo) {
    G4ExceptionDescription description;
    description << G4String("PreUserTrackingAction: ") + "NULL PrimaryParticleInformation pointer for current G4Step (trackID "
                << aTrack.GetTrackID() << ", track pos: "<<aTrack.GetPosition() << ", mom: "<<aTrack.GetMomentum()
                << ", parentID " << aTrack.GetParentID() << ")";
    G4Exception("iGeant4::TrackProcessorUserActionBase", "NoPPInfo", FatalException, description);
    return; // The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }

  // get base ISFParticle and link to TrackInformation
  auto* baseISP = ppInfo->GetISFParticle();
  if (!baseISP) {
    G4ExceptionDescription description;
    description << G4String("PreUserTrackingAction: ") + "No ISFParticle associated with primary particle (trackID: "
                << aTrack.GetTrackID() << ", track pos: "<<aTrack.GetPosition() << ", mom: "<<aTrack.GetMomentum()
                << ", parentID " << aTrack.GetParentID() << ")";
    G4Exception("iGeant4::TrackProcessorUserActionBase", "NoISFParticle", FatalException, description);
    return; // The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }

  auto* truthBinding = baseISP->getTruthBinding();
  if (!truthBinding) {
    G4ExceptionDescription description;
    description << G4String("PreUserTrackingAction: ") + "No ISF::TruthBinding associated with primary particle (trackID: "
                << aTrack.GetTrackID() << ", track pos: "<<aTrack.GetPosition() << ", mom: "<<aTrack.GetMomentum()
                << ", parentID " << aTrack.GetParentID() << ")";
    G4Exception("iGeant4::TrackProcessorUserActionBase", "NoISFTruthBinding", FatalException, description);
    return; // The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }

  int regenerationNr = ppInfo->GetRegenerationNr();
  auto* primaryTruthParticle = truthBinding->getGenerationZeroTruthParticle();
  auto* generationZeroTruthParticle = truthBinding->getGenerationZeroTruthParticle();
  auto* currentlyTracedHepPart = truthBinding->getTruthParticle();
  auto classification = classify(primaryTruthParticle,
                                 generationZeroTruthParticle,
                                 currentlyTracedHepPart,
                                 regenerationNr);

  auto* newTrackInfo = ::iGeant4::ISFG4Helper::attachTrackInfoToNewG4Track(aTrack,
                                                                 *baseISP,
                                                                 classification,
                                                                 generationZeroTruthParticle );
  newTrackInfo->SetRegenerationNr(regenerationNr);

  setCurrentParticle(const_cast<ISF::ISFParticle*>(baseISP),
                     primaryTruthParticle,
                     currentlyTracedHepPart);
  return;
}

void TrackProcessorUserActionBase::setupSecondary(const G4Track& aTrack)
{
  auto* trackInfo = ::iGeant4::ISFG4Helper::getISFTrackInfo(aTrack);

  // why does TrackInformation return *const* GenParticle and ISFParticle objects!?
  auto* currentlyTracedTruthParticle = const_cast<HepMC::GenParticle*>( trackInfo->GetHepMCParticle() );
  auto* primaryTruthParticle = const_cast<HepMC::GenParticle*>( trackInfo->GetPrimaryHepMCParticle() );
  auto* baseISFParticle = const_cast<ISF::ISFParticle*>( trackInfo->GetBaseISFParticle() );

  setCurrentParticle(baseISFParticle, primaryTruthParticle, currentlyTracedTruthParticle);
  return;
}

void TrackProcessorUserActionBase::setCurrentParticle(ISF::ISFParticle* baseISFParticle,
                                                      HepMC::GenParticle* truthPrimary,
                                                      HepMC::GenParticle* truthCurrentlyTraced)
{
  m_curBaseISP = baseISFParticle;
  m_eventInfo->SetCurrentPrimary( truthPrimary );
  m_eventInfo->SetCurrentlyTraced( truthCurrentlyTraced );
  return;
}

/// Classify the particle represented by the given set of truth links
TrackClassification TrackProcessorUserActionBase::classify(const HepMC::GenParticle* primaryTruthParticle,
                                                           const HepMC::GenParticle* generationZeroTruthParticle,
                                                           const HepMC::GenParticle* currentlyTracedHepPart,
                                                           int regenerationNumber) const
{
  // if particle points to a non-zero truth particle it can not just be a 'simple' Secondary
  if (currentlyTracedHepPart) {
    if (currentlyTracedHepPart==primaryTruthParticle) {
      return Primary;
    }
    else if (generationZeroTruthParticle==primaryTruthParticle && regenerationNumber>0) {
      return RegeneratedPrimary;
    }
    else {
      return RegisteredSecondary;
    }
  }

  return Secondary;
}



void TrackProcessorUserActionBase::PostUserTrackingAction(const G4Track*)
{
  m_curBaseISP = nullptr;
  return;
}

} // namespace iGeant4

} // namespace G4UA
