/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SteppingValidationTool.h"
namespace G4UA{ 


  SteppingValidationTool::SteppingValidationTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<SteppingValidation>(type, name, parent){
  }
  std::unique_ptr<SteppingValidation>  SteppingValidationTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    return std::make_unique<SteppingValidation>();
  }
  StatusCode SteppingValidationTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IG4RunActionTool::interfaceID()) {
      *ppvIf = (IG4RunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4SteppingActionTool::interfaceID()) {
      *ppvIf = (IG4SteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<SteppingValidation>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
