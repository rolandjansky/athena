/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkG4UserActions/GeantFollowerMSTool.h"
#include "TrkG4UserActions/IGeantFollowerMSHelper.h"

namespace G4UA
{

  GeantFollowerMSTool::GeantFollowerMSTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : UserActionToolBase<GeantFollowerMS>(type, name, parent)
  {
    declareProperty("HelperTool", m_config.helper);
  }

  std::unique_ptr<GeantFollowerMS>
  GeantFollowerMSTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a GeantFollowerMS action");
    auto action = std::make_unique<GeantFollowerMS>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
