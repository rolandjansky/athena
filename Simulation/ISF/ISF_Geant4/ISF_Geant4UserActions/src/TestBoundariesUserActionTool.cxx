/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TestBoundariesUserActionTool.h"

namespace G4UA
{

  namespace iGeant4
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
      auto action = CxxUtils::make_unique<TestBoundariesUserAction>();
      actionList.runActions.push_back( action.get() );
      actionList.steppingActions.push_back( action.get() );
      return action;
    }

  } // namespace iGeant4

} // namespace G4UA
