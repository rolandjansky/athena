/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkG4UserActions/MaterialStepRecorderTool.h"

namespace G4UA
{

  MaterialStepRecorderTool::MaterialStepRecorderTool(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent)
    : UserActionToolBase<MaterialStepRecorder>(type, name, parent)
  {
  }

  std::unique_ptr<MaterialStepRecorder>
  MaterialStepRecorderTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a MaterialStepRecorder action");
    auto action = std::make_unique<MaterialStepRecorder>();
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
