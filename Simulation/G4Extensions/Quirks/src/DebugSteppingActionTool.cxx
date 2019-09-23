/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DebugSteppingActionTool.h"

namespace G4UA
{

  DebugSteppingActionTool::DebugSteppingActionTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
    : UserActionToolBase<DebugSteppingAction>(type, name, parent)
  {
    declareProperty("DebugStep", m_config.step);
    declareProperty("NumSteps", m_config.numSteps);
  }

  std::unique_ptr<DebugSteppingAction>
  DebugSteppingActionTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a DebugSteppingAction");
    auto action = std::make_unique<DebugSteppingAction>(m_config);
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
