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
      return ActionToolBase<TestBoundariesUserAction>::queryInterface(riid, ppvIf);
    }
  } // namespace iGeant4
} // namespace G4UA 
