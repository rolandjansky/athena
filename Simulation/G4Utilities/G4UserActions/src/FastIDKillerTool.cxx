/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/FastIDKillerTool.h"

namespace G4UA
{

  FastIDKillerTool::FastIDKillerTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : ActionToolBaseReport<FastIDKiller>(type, name, parent)
  {
    declareInterface<IG4RunActionTool>(this);
    declareInterface<IG4SteppingActionTool>(this);

    declareProperty("R", m_config.R);
    declareProperty("Z", m_config.Z);
  }

  std::unique_ptr<FastIDKiller> FastIDKillerTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return std::make_unique<FastIDKiller>(m_config);
  }

  StatusCode FastIDKillerTool::finalize()
  {
    mergeReports();
    ATH_MSG_INFO( "Fast Inner Detector Killer killed " << m_report.killCount <<
                  " particles during this run." );
    return StatusCode::SUCCESS;
  }

} // namespace G4UA
