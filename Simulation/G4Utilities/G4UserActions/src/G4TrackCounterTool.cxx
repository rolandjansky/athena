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
    : ActionToolBaseReport<G4TrackCounter>(type, name, parent)
  {
    declareInterface<IBeginEventActionTool>(this);
    declareInterface<IPreTrackingActionTool>(this);
  }

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode G4TrackCounterTool::initialize()
  {
    ATH_MSG_DEBUG("initialize");
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Merge results from all threads
  //---------------------------------------------------------------------------
  StatusCode G4TrackCounterTool::finalize()
  {
    ATH_MSG_DEBUG("finalize");

    mergeReports();

    // Report the totals
    ATH_MSG_INFO("nEvents          " << m_report.nEvents);
    ATH_MSG_INFO("nPrimaryTracks   " << m_report.nPrimaryTracks);
    ATH_MSG_INFO("nSecondaryTracks " << m_report.nSecondaryTracks);
    ATH_MSG_INFO("n50MeVTracks     " << m_report.n50MeVTracks);

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

}
