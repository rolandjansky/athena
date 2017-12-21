/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/HIPKillerTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  HIPKillerTool::HIPKillerTool(const std::string& type, const std::string& name,
                               const IInterface* parent)
    : UserActionToolBase<HIPKiller>(type, name, parent)
  {
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<HIPKiller>
  HIPKillerTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Making a HIPKiller action");
    auto action = std::make_unique<HIPKiller>();
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
