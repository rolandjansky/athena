/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CosmicPerigeeActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  CosmicPerigeeActionTool::CosmicPerigeeActionTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
    : UserActionToolBase<CosmicPerigeeAction>(type, name, parent)
  {
    declareProperty("pMinPrimary", m_config.pMinPrimary);
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<CosmicPerigeeAction>
  CosmicPerigeeActionTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Creating CosmicPerigeeAction");
    auto action = std::make_unique<CosmicPerigeeAction>(m_config);
    actionList.eventActions.push_back( action.get() );
    actionList.trackingActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

}
