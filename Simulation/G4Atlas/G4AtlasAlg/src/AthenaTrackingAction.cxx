/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaTrackingAction.h"

#include <iostream>

#include "G4Event.hh"
#include "G4EventManager.hh"

#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruthBase/AtlasTrajectory.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  AthenaTrackingAction::AthenaTrackingAction(MSG::Level lvl,
                                             int secondarySavingLevel)
    : m_msg("AthenaTrackingAction")
    , m_secondarySavingLevel(secondarySavingLevel)
  {
    m_msg.get().setLevel(lvl);
  }

  //---------------------------------------------------------------------------
  // Pre-tracking action.
  //---------------------------------------------------------------------------
  void AthenaTrackingAction::PreUserTrackingAction(const G4Track* track)
  {
    ATH_MSG_DEBUG("Starting to track a new particle");

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
      EventInformation* eventInfo = static_cast<EventInformation*>
        (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
      if (trackHelper.IsPrimary()) eventInfo->SetCurrentPrimary(part);
      eventInfo->SetCurrentlyTraced(part);
    }

    // Condition for creating a trajectory object to store truth.
    if (trackHelper.IsPrimary() ||
        (trackHelper.IsRegisteredSecondary() && m_secondarySavingLevel>1) ||
        (trackHelper.IsSecondary() && m_secondarySavingLevel>2))
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
  void AthenaTrackingAction::PostUserTrackingAction(const G4Track* /*track*/)
  {
    ATH_MSG_DEBUG("Finished tracking a particle");

    // We are done tracking this particle, so reset the trajectory.
    // TODO: consider caching the tracking manager once to reduce overhead.
    G4EventManager::GetEventManager()->GetTrackingManager()->
      SetStoreTrajectory(false);
  }

} // namespace G4UA
