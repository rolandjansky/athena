/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4TrackCounterTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  G4TrackCounterTool::
  G4TrackCounterTool(const std::string& type, const std::string& name,
                     const IInterface* parent)
    : UserActionToolBase<G4TrackCounter>(type, name, parent)
  {
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode G4TrackCounterTool::initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Merge results from all threads
  //---------------------------------------------------------------------------
  StatusCode G4TrackCounterTool::finalize()
  {
    ATH_MSG_DEBUG( "Finalizing " << name() );

    // Accumulate the results across threads
    G4TrackCounter::Report report;
    m_actions.accumulate(report, &G4TrackCounter::getReport,
                         &G4TrackCounter::Report::merge);

    // Report the totals
    ATH_MSG_INFO("nEvents          " << report.nEvents);
    ATH_MSG_INFO("nPrimaryTracks   " << report.nPrimaryTracks);
    ATH_MSG_INFO("nSecondaryTracks " << report.nSecondaryTracks);
    ATH_MSG_INFO("n50MeVTracks     " << report.n50MeVTracks);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<G4TrackCounter>
  G4TrackCounterTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Making a G4TrackCounter action");
    auto action = std::make_unique<G4TrackCounter>();
    actionList.eventActions.push_back( action.get() );
    actionList.trackingActions.push_back( action.get() );
    return action;
  }

}
