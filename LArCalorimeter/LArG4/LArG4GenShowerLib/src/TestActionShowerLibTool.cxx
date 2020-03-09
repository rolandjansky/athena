/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4GenShowerLib/TestActionShowerLibTool.h"

namespace G4UA
{

  //----------------------------------------------------------------------------
  TestActionShowerLibTool::TestActionShowerLibTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
    : UserActionToolBase<TestActionShowerLib>(type, name, parent)
  {}

  //----------------------------------------------------------------------------
  std::unique_ptr<TestActionShowerLib>
  TestActionShowerLibTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a TestActionShowerLib");
    auto action = std::make_unique<TestActionShowerLib>();
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
