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
