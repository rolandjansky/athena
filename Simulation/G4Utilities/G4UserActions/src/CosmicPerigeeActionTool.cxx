/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/CosmicPerigeeActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  CosmicPerigeeActionTool::CosmicPerigeeActionTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
    : ActionToolBase<CosmicPerigeeAction>(type, name, parent)
  {
    declareProperty("pMinPrimary",m_config.pMinPrimary);
  }
  

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<CosmicPerigeeAction>
  CosmicPerigeeActionTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<CosmicPerigeeAction>(m_config);
    return std::move(action);
  }

  //---------------------------------------------------------------------------
  // Query interface
  //---------------------------------------------------------------------------
  StatusCode CosmicPerigeeActionTool::queryInterface(const InterfaceID& riid, void** ppvIf)
  {
    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    
    if(riid == IEndEventActionTool::interfaceID()) {
      *ppvIf = (IEndEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    
    if(riid == IBeginEventActionTool::interfaceID()) {
      *ppvIf = (IBeginEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    
    if(riid == IPreTrackingActionTool::interfaceID()) {
      *ppvIf = (IPreTrackingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    
    return ActionToolBase<CosmicPerigeeAction>::queryInterface(riid, ppvIf);
  }
  
}
