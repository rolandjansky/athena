/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrkG4UserActions/MaterialStepRecorderTool.h"

namespace G4UA{ 

  MaterialStepRecorderTool::MaterialStepRecorderTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<MaterialStepRecorder>(type, name, parent){
  }
  std::unique_ptr<MaterialStepRecorder>  MaterialStepRecorderTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<MaterialStepRecorder>();
    return std::move(action);
  }
  StatusCode MaterialStepRecorderTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4RunActionTool::interfaceID()) {
      *ppvIf = (IG4RunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4SteppingActionTool::interfaceID()) {
      *ppvIf = (IG4SteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<MaterialStepRecorder>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
