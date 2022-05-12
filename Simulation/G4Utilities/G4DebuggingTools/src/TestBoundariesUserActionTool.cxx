/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TestBoundariesUserActionTool.h"

namespace G4UA
{

  TestBoundariesUserActionTool::TestBoundariesUserActionTool(const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent)
    : UserActionToolBase<TestBoundariesUserAction>(type, name, parent)
  {
  }

  std::unique_ptr<TestBoundariesUserAction>
  TestBoundariesUserActionTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a TestBoundariesUserAction");
    auto action = std::make_unique<TestBoundariesUserAction>();
    actionList.runActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
