/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/FastIDKillerTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  FastIDKillerTool::FastIDKillerTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : UserActionToolBase<FastIDKiller>(type, name, parent)
  {
    declareProperty("R", m_config.R);
    declareProperty("Z", m_config.Z);
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<FastIDKiller>
  FastIDKillerTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a FastIDKiller action");
    auto action = std::make_unique<FastIDKiller>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

  //---------------------------------------------------------------------------
  StatusCode FastIDKillerTool::finalize()
  {
    // Accumulate results across threads
    FastIDKiller::Report report;
    m_actions.accumulate(report, &FastIDKiller::getReport,
                         &FastIDKiller::Report::merge);
    ATH_MSG_INFO( "Fast Inner Detector Killer killed " << report.killCount <<
                  " particles during this run." );
    return StatusCode::SUCCESS;
  }

} // namespace G4UA
