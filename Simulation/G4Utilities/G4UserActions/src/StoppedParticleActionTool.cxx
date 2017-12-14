/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/StoppedParticleActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  StoppedParticleActionTool::StoppedParticleActionTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent)
    : UserActionToolBase<StoppedParticleAction>(type, name, parent)
  {
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<StoppedParticleAction>
  StoppedParticleActionTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Making a StoppedParticleAction");
    auto action = std::make_unique<StoppedParticleAction>();
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
