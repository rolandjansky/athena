/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CheckActivationTool.h"

namespace G4UA
{

  CheckActivationTool::CheckActivationTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : UserActionToolBase<CheckActivation>(type, name, parent)
  {
  }

  std::unique_ptr<CheckActivation>
  CheckActivationTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a CheckActivation action");
    auto action = std::make_unique<CheckActivation>();
    actionList.eventActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
