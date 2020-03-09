/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StepNtupleTool.h"

namespace G4UA
{

  StepNtupleTool::StepNtupleTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
    : UserActionToolBase<StepNtuple>(type, name, parent)
  {}

  std::unique_ptr<StepNtuple>
  StepNtupleTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a StepNtuple action");
    auto action = std::make_unique<StepNtuple>();
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
