/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/ScoringPlaneTool.h"


namespace G4UA
{

  ScoringPlaneTool::ScoringPlaneTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : ActionToolBase<ScoringPlane>(type, name, parent)
  {
    declareInterface<IG4RunActionTool>(this);
    declareInterface<IG4SteppingActionTool>(this);
    declareInterface<IG4EventActionTool>(this);
    declareProperty("Plane", m_config.plane);
    declareProperty("PKill", m_config.pkill);
    declareProperty("FName", m_config.fname);
  }

  std::unique_ptr<ScoringPlane> ScoringPlaneTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return std::make_unique<ScoringPlane>(m_config);
  }

} // namespace G4UA
