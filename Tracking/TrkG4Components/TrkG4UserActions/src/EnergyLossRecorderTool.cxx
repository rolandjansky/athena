/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrkG4UserActions/EnergyLossRecorderTool.h"

namespace G4UA{ 
  
  
  EnergyLossRecorderTool::EnergyLossRecorderTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<EnergyLossRecorder>(type, name, parent), m_config(){

    declareProperty("cylinders",m_config.cylinders);
    declareProperty("minEta",m_config.minEta);
    declareProperty("maxEta",m_config.maxEta);
    declareProperty("momentum",m_config.momentum);
    declareProperty("pdg",m_config.pdg);
    declareProperty("usePt",m_config.usePt);
    declareProperty("useX0",m_config.useX0prop);
    declareProperty("onionMode",m_config.onionModeprop);
    declareProperty("keepAlive",m_config.keepAliveprop);
    declareProperty("R",m_config.Rvec);
    declareProperty("Z",m_config.Zvec);
  }
  std::unique_ptr<EnergyLossRecorder>  EnergyLossRecorderTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<EnergyLossRecorder>(m_config);
    return std::move(action);
  }
  StatusCode EnergyLossRecorderTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
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
    return ActionToolBase<EnergyLossRecorder>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
