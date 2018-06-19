/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrkG4UserActions/GeantFollowerTool.h"

namespace G4UA
{

  GeantFollowerTool::GeantFollowerTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent)
    : UserActionToolBase<GeantFollower>(type, name, parent)
  {
    declareProperty("HelperTool", m_config.helper);
  }

  std::unique_ptr<GeantFollower>
  GeantFollowerTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a GeantFollower action");
    auto action = CxxUtils::make_unique<GeantFollower>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
