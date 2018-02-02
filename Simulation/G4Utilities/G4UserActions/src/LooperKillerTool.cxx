/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/LooperKillerTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  LooperKillerTool::LooperKillerTool(const std::string& type, const std::string& name,
                                 const IInterface* parent)
    : UserActionToolBase<LooperKiller>(type, name, parent)
  {
    declareProperty("MaxSteps", m_config.MaxSteps);
    declareProperty("PrintSteps",m_config.PrintSteps);
    declareProperty("VerboseLevel", m_config.VerboseLevel);
    declareProperty("AbortEvent", m_config.AbortEvent);
    declareProperty("SetError", m_config.SetError);
  }

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode LooperKillerTool::initialize()
  {
    ATH_MSG_DEBUG("initialize");
    return StatusCode::SUCCESS;
  }

  StatusCode LooperKillerTool::finalize()
  {
    ATH_MSG_DEBUG("finalize");

    // Accumulate the results across threads
    LooperKiller::Report report;
    m_actions.accumulate(report, &LooperKiller::getReport,
                         &LooperKiller::Report::merge);

    ATH_MSG_INFO( "******* Report from "<< name()<< " *******");
    ATH_MSG_INFO(" Set to kill tracks over " << m_config.MaxSteps << " steps");
    ATH_MSG_INFO(" and give " << m_config.PrintSteps << " steps of verbose output");
    ATH_MSG_INFO(" We killed " << report.killed_tracks << " tracks this run.");
    ATH_MSG_INFO(" Was set to " << (m_config.AbortEvent?"":"not ") << "abort events and ");
    ATH_MSG_INFO( (m_config.SetError?"":"not ") << "set an error state." );

    return StatusCode::SUCCESS;
  }


  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<LooperKiller>
  LooperKillerTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Making a LooperKiller action");
    auto action =  std::make_unique<LooperKiller>(m_config);
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

}
