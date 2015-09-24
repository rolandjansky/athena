/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/G4TrackCounterTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  G4TrackCounterTool::
  G4TrackCounterTool(const std::string& type, const std::string& name,
                     const IInterface* parent)
    : ActionToolBase<G4TrackCounter>(type, name, parent)
  {}

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode G4TrackCounterTool::initialize()
  {
    ATH_MSG_INFO("initialize");
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Merge results from all threads
  //---------------------------------------------------------------------------
  StatusCode G4TrackCounterTool::finalize()
  {
    ATH_MSG_INFO("finalize");

    // Loop over the thread-local tools and sum their counts
    G4TrackCounter::TrackCounts totalCounts;
    for(auto tidAction : actions()) {
      auto& counts = tidAction.second->getCounts();
      totalCounts.nEvents += counts.nEvents;
      totalCounts.nPrimaryTracks += counts.nPrimaryTracks;
      totalCounts.nSecondaryTracks += counts.nSecondaryTracks;
      totalCounts.n50MeVTracks += counts.n50MeVTracks;
    }

    // Report the totals
    ATH_MSG_INFO("nEvents          " << totalCounts.nEvents);
    ATH_MSG_INFO("nPrimaryTracks   " << totalCounts.nPrimaryTracks);
    ATH_MSG_INFO("nSecondaryTracks " << totalCounts.nSecondaryTracks);
    ATH_MSG_INFO("n50MeVTracks     " << totalCounts.n50MeVTracks);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<G4TrackCounter>
  G4TrackCounterTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<G4TrackCounter>();
    return std::move(action);
  }

  //---------------------------------------------------------------------------
  // Query interface
  //---------------------------------------------------------------------------
  StatusCode G4TrackCounterTool::queryInterface(const InterfaceID& riid,
                                                void** ppvIf)
  {
    if(riid == IBeginEventActionTool::interfaceID()) {
      *ppvIf = (IBeginEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IPreTrackingActionTool::interfaceID()) {
      *ppvIf = (IPreTrackingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<G4TrackCounter>::queryInterface(riid, ppvIf);
  }

}
