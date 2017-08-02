/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/StoppedParticleActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  StoppedParticleActionTool::StoppedParticleActionTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent)
    : ActionToolBase<StoppedParticleAction>(type, name, parent)
  {}

  //---------------------------------------------------------------------------
  std::unique_ptr<StoppedParticleAction> StoppedParticleActionTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<StoppedParticleAction>();
  }

  //---------------------------------------------------------------------------
  StatusCode StoppedParticleActionTool::queryInterface(const InterfaceID& riid,
                                                       void** ppvIf)
  {
    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } return ActionToolBase<StoppedParticleAction>::queryInterface(riid, ppvIf);
  }

} // namespace G4UA
