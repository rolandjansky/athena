/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/FluxRecorderTool.h"

namespace G4UA
{

  FluxRecorderTool::FluxRecorderTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : UserActionToolBase<FluxRecorder>(type, name, parent)
  {
  }

  std::unique_ptr<FluxRecorder>
  FluxRecorderTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a FluxRecorder action");
    auto action = std::make_unique<FluxRecorder>();
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
