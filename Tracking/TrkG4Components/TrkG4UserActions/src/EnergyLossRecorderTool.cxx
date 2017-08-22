/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrkG4UserActions/EnergyLossRecorderTool.h"
#include "TrkValInterfaces/IPositionMomentumWriter.h"

namespace G4UA{ 
  
  
  EnergyLossRecorderTool::EnergyLossRecorderTool(const std::string& type, const std::string& name,const IInterface* parent)
    : ActionToolBase<EnergyLossRecorder>(type, name, parent), m_config()
    , m_pmWriter("")
  {
    declareProperty("PositionMomentumWriter", m_pmWriter, "");
  }
  StatusCode EnergyLossRecorderTool::initialize()
  {
    if(!m_pmWriter.empty())
      {
        ATH_CHECK(m_pmWriter.retrieve());
        m_config.pmWriter = &(*m_pmWriter);
      }
    return StatusCode::SUCCESS;
  }
  std::unique_ptr<EnergyLossRecorder>  EnergyLossRecorderTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<EnergyLossRecorder>(m_config);
    return std::move(action);
  }
  StatusCode EnergyLossRecorderTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
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
    return ActionToolBase<EnergyLossRecorder>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
