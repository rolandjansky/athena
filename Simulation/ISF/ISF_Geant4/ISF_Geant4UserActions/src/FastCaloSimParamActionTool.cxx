/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "ISF_Geant4UserActions/FastCaloSimParamActionTool.h"

namespace G4UA{ 

  FastCaloSimParamActionTool::FastCaloSimParamActionTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<FastCaloSimParamAction>(type, name, parent), m_config(){
    declareProperty("shift_lar_subhit",m_config.shift_lar_subhit, "");
    declareProperty("shorten_lar_step",m_config.shorten_lar_step, "");
    
  }

  std::unique_ptr<FastCaloSimParamAction>  FastCaloSimParamActionTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<FastCaloSimParamAction>(m_config);
    return std::move(action);
  }
  
  StatusCode FastCaloSimParamActionTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IBeginRunActionTool::interfaceID()) {
      *ppvIf = (IBeginRunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IEndRunActionTool::interfaceID()) {
      *ppvIf = (IEndRunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IBeginEventActionTool::interfaceID()) {
      *ppvIf = (IBeginEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IEndEventActionTool::interfaceID()) {
      *ppvIf = (IEndEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<FastCaloSimParamAction>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
