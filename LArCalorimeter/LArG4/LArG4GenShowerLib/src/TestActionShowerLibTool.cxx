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
    return ActionToolBase<TestActionShowerLib>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
