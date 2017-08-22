/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TestActionEHistTool.h"

namespace G4UA{ 


  TestActionEHistTool::TestActionEHistTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<TestActionEHist>(type, name, parent), m_config(){

    declareProperty("ROOTFileName",m_config.name);
    declareProperty("CaloDepth",m_config.dCALO);
    declareProperty("BeamPipeDepth",m_config.dBeam);
    declareProperty("InDetDepth",m_config.dIDET);
    declareProperty("MuonDepth",m_config.dMUON);
    declareProperty("MaxHists",m_config.maxhists);
    declareProperty("DetailDepth",m_config.dDetail);
    
  }

  std::unique_ptr<TestActionEHist>  TestActionEHistTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<TestActionEHist>(m_config);
    return std::move(action);
  }

  StatusCode TestActionEHistTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IG4TrackingActionTool::interfaceID()) {
      *ppvIf = (IG4TrackingActionTool*) this;
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
    return ActionToolBase<TestActionEHist>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
