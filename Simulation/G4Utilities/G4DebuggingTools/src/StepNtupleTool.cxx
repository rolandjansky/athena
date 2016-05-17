/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "StepNtupleTool.h"
namespace G4UA{ 


  StepNtupleTool::StepNtupleTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<StepNtuple>(type, name, parent){
  }
  std::unique_ptr<StepNtuple>  StepNtupleTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<StepNtuple>();
    return std::move(action);
  }
  StatusCode StepNtupleTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
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
    if(riid == IBeginRunActionTool::interfaceID()) {
      *ppvIf = (IBeginRunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<StepNtuple>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
