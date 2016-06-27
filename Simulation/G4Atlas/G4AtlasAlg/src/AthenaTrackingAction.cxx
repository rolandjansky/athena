/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaTrackingAction.h"

#include "MCTruth/EventInformation.h"
#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruthBase/AtlasTrajectory.h"
#include "MCTruthBase/TruthStrategyManager.h"

#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"
#include "G4EventManager.hh"

#include <iostream>

AthenaTrackingAction::AthenaTrackingAction(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
  : UserActionBase(type, name, parent)
{
}

void AthenaTrackingAction::PreTracking(const G4Track* inTrack)
{
  // Retrieve the saving level for secondaries.
  // Why isn't this handled via normal configuration?
  static int ilevel =
    TruthStrategyManager::GetStrategyManager()->GetSecondarySavingLevel();
  //std::cout<<" this is AthenaTrackingAction::PreUserTrackingAction"<<std::endl;

  // Use the TrackHelper code to identify the kind of particle.
  TrackHelper trackHelper(inTrack);
  if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary())
    {
      // Why a const_cast???
      // This is an ugly way to communicate the GenParticle...
      HepMC::GenParticle* part =
        const_cast<HepMC::GenParticle*>( trackHelper.GetTrackInformation()->
                                         GetHepMCParticle() );
      EventInformation* eventInfo =
        TruthStrategyManager::GetStrategyManager()->GetEventInformation();
      if (trackHelper.IsPrimary()) eventInfo->SetCurrentPrimary(part);
      eventInfo->SetCurrentlyTraced(part);
    }
  if (trackHelper.IsPrimary() ||
      (trackHelper.IsRegisteredSecondary() && ilevel>1) ||
      (trackHelper.IsSecondary() && ilevel>2))
    {
      // Create a new AtlasTrajectory and store it.
      AtlasTrajectory* temp = new AtlasTrajectory(inTrack);
      m_fpTrackingManager->SetStoreTrajectory(true);
      m_fpTrackingManager->SetTrajectory(temp);
    }
}


void AthenaTrackingAction::PostTracking(const G4Track* /*outTrack*/)
{
  //std::cout <<"AthenaTrackingAction::PostUserTrackingAction"<<std::endl;
  // We are done tracking this particle, so reset the trajectory.
  m_fpTrackingManager->SetStoreTrajectory(false);
}

// TODO: move to use declareInterface instead.
StatusCode AthenaTrackingAction::queryInterface(const InterfaceID& riid,
                                                void** ppvInterface)
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


//=============================================================================
// New design for multi-threading (V2 migration) follows.
//=============================================================================

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  AthenaTrackingAction::AthenaTrackingAction(MSG::Level lvl)
    : m_msg("AthenaTrackingAction")
  {
    m_msg.get().setLevel(lvl);
  }

  //---------------------------------------------------------------------------
  // Pre-tracking action.
  //---------------------------------------------------------------------------
  void AthenaTrackingAction::preTracking(const G4Track* track)
  {
    ATH_MSG_DEBUG("Starting to track a new particle");

    // Retrieve the saving level for secondaries.
    // TODO: use a more normal configuration mechanism for this.
    static int ilevel =
      TruthStrategyManager::GetStrategyManager()->GetSecondarySavingLevel();

    // Use the TrackHelper code to identify the kind of particle.
    TrackHelper trackHelper(track);

    // Condition for storing the GenParticle in the EventInformation for later.
    if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary())
    {
      // Why a const_cast???
      // This is an ugly way to communicate the GenParticle...
      HepMC::GenParticle* part =
        const_cast<HepMC::GenParticle*>( trackHelper.GetTrackInformation()->
                                         GetHepMCParticle() );

      // Assign the GenParticle to the EventInformation.
      EventInformation* eventInfo =
        TruthStrategyManager::GetStrategyManager()->GetEventInformation();
      if (trackHelper.IsPrimary()) eventInfo->SetCurrentPrimary(part);
      eventInfo->SetCurrentlyTraced(part);
    }

    // Condition for creating a trajectory object to store truth.
    if (trackHelper.IsPrimary() ||
        (trackHelper.IsRegisteredSecondary() && ilevel>1) ||
        (trackHelper.IsSecondary() && ilevel>2))
    {
      ATH_MSG_DEBUG("Preparing an AtlasTrajectory for saving truth");

      // Create a new AtlasTrajectory for this particle
      AtlasTrajectory* trajectory = new AtlasTrajectory(track);

      // Assign the trajectory to the tracking manager.
      // TODO: consider caching the tracking manager once to reduce overhead.
      auto trkMgr = G4EventManager::GetEventManager()->GetTrackingManager();
      //trajectory->setTrackingManager(trkMgr);
      trkMgr->SetStoreTrajectory(true);
      trkMgr->SetTrajectory(trajectory);
    }
  }

  //---------------------------------------------------------------------------
  // Post-tracking action.
  //---------------------------------------------------------------------------
  void AthenaTrackingAction::postTracking(const G4Track* /*track*/)
  {
    ATH_MSG_DEBUG("Finished tracking a particle");

    // We are done tracking this particle, so reset the trajectory.
    // TODO: consider caching the tracking manager once to reduce overhead.
    G4EventManager::GetEventManager()->GetTrackingManager()->
      SetStoreTrajectory(false);
  }

} // namespace G4UA
