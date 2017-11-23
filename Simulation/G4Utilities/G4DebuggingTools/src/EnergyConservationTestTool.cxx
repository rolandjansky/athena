/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "EnergyConservationTestTool.h"

namespace G4UA{ 
  
  EnergyConservationTestTool::EnergyConservationTestTool(const std::string& type, const std::string& name,const IInterface* parent):
    
    ActionToolBase<EnergyConservationTest>(type, name, parent){
  }

  std::unique_ptr<EnergyConservationTest>  EnergyConservationTestTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<EnergyConservationTest>();
    return std::move(action);
  }
  
  StatusCode EnergyConservationTestTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IG4TrackingActionTool::interfaceID()) {
      *ppvIf = (IG4TrackingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } if(riid == IG4SteppingActionTool::interfaceID()) {
      *ppvIf = (IG4SteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } return ActionToolBase<EnergyConservationTest>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
