/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeantFollowerTool.h"

ActsGeantFollowerTool::ActsGeantFollowerTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
  : G4UA::UserActionToolBase<ActsGeantFollower>(type, name, parent)
{}

std::unique_ptr<ActsGeantFollower>
ActsGeantFollowerTool::makeAndFillAction(G4UA::G4AtlasUserActions& actionList)
{
  ATH_MSG_DEBUG("Constructing a GeantFollower action");
  auto action = std::make_unique<ActsGeantFollower>();
  actionList.runActions.push_back( action.get() );
  actionList.eventActions.push_back( action.get() );
  actionList.steppingActions.push_back( action.get() );
  return action;
}
