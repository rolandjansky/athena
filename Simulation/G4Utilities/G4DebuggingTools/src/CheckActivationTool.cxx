/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "CheckActivationTool.h"

namespace G4UA{ 
  
  
  CheckActivationTool::CheckActivationTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<CheckActivation>(type, name, parent){
  }
  
  std::unique_ptr<CheckActivation>  CheckActivationTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<CheckActivation>();
    return std::move(action);
  }

  StatusCode CheckActivationTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<CheckActivation>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
