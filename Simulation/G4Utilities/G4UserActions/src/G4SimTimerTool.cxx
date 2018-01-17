/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4SimTimerTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  G4SimTimerTool::G4SimTimerTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
    : UserActionToolBase<G4SimTimer>(type, name, parent)
  {
  }

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode G4SimTimerTool::initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Merge results from all threads
  //---------------------------------------------------------------------------
  StatusCode G4SimTimerTool::finalize()
  {
    ATH_MSG_DEBUG( "Finalizing " << name() );

    // Accumulate results across threads
    G4SimTimer::Report report;
    m_actions.accumulate(report, &G4SimTimer::getReport,
                         &G4SimTimer::Report::merge);

    // Report the results
    auto meanSigma = report.meanAndSigma();
    ATH_MSG_INFO("Finalized timing results for " << report.nEvent <<
                 " events (not all events used)");
    ATH_MSG_INFO("Average time per event was " <<
                 std::setprecision(4) << meanSigma.first << " +- " <<
                 std::setprecision(4) << meanSigma.second);
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<G4SimTimer>
  G4SimTimerTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Making a G4SimTimer action");
    auto action = std::make_unique<G4SimTimer>();
    actionList.eventActions.push_back( action.get() );
    return action;
  }

}
