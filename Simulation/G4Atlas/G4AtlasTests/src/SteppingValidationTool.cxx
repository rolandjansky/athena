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
    
    if(riid == IBeginRunActionTool::interfaceID()) {
      *ppvIf = (IBeginRunActionTool*) this;
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
    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<SteppingValidation>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
