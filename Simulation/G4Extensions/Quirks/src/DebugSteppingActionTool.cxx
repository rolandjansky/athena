/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "DebugSteppingActionTool.h"
namespace G4UA{ 

  DebugSteppingActionTool::DebugSteppingActionTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<DebugSteppingAction>(type, name, parent), m_config(){
    declareProperty("DebugStep", m_config.step);
    declareProperty("NumSteps", m_config.numSteps);
  }
  std::unique_ptr<DebugSteppingAction>  DebugSteppingActionTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<DebugSteppingAction>(m_config);
    return std::move(action);
  }
  StatusCode DebugSteppingActionTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IG4SteppingActionTool::interfaceID()) {
      *ppvIf = (IG4SteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<DebugSteppingAction>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
