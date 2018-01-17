/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/ScoringPlaneTool.h"


namespace G4UA
{

  ScoringPlaneTool::ScoringPlaneTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : UserActionToolBase<ScoringPlane>(type, name, parent)
  {
    declareProperty("Plane", m_config.plane);
    declareProperty("PKill", m_config.pkill);
    declareProperty("FName", m_config.fname);
  }

  std::unique_ptr<ScoringPlane>
  ScoringPlaneTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Making a ScoringPlane action");
    auto action = std::make_unique<ScoringPlane>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
