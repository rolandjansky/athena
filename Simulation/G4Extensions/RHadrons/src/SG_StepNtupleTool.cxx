/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "SG_StepNtupleTool.h"

namespace G4UA{ 
  
  
  SG_StepNtupleTool::SG_StepNtupleTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<SG_StepNtuple>(type, name, parent){
  }
  std::unique_ptr<SG_StepNtuple>  SG_StepNtupleTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<SG_StepNtuple>();
    return std::move(action);
  }
  StatusCode SG_StepNtupleTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
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
    return ActionToolBase<SG_StepNtuple>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
