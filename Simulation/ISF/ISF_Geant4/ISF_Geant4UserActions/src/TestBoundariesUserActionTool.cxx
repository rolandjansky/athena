/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TestBoundariesUserActionTool.h"

namespace G4UA{ 

  namespace iGeant4{
    
    TestBoundariesUserActionTool::TestBoundariesUserActionTool(const std::string& type, const std::string& name,const IInterface* parent):
      ActionToolBase<TestBoundariesUserAction>(type, name, parent){
    }
    std::unique_ptr<TestBoundariesUserAction>  TestBoundariesUserActionTool::makeAction(){
      ATH_MSG_DEBUG("makeAction");
      auto action = CxxUtils::make_unique<TestBoundariesUserAction>();
      return std::move(action);
    }
    StatusCode TestBoundariesUserActionTool::queryInterface(const InterfaceID& riid, void** ppvIf){
      
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
      return ActionToolBase<TestBoundariesUserAction>::queryInterface(riid, ppvIf);
    }
  } // namespace iGeant4
} // namespace G4UA 
