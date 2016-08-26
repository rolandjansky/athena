/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "LArG4GenShowerLib/TestActionShowerLibTool.h"

namespace G4UA{ 


  TestActionShowerLibTool::TestActionShowerLibTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<TestActionShowerLib>(type, name, parent){
  }
  std::unique_ptr<TestActionShowerLib>  TestActionShowerLibTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<TestActionShowerLib>();
  }
  StatusCode TestActionShowerLibTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
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
    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<TestActionShowerLib>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
