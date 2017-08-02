/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/ScoringPlaneTool.h"


namespace G4UA
{

  ScoringPlaneTool::ScoringPlaneTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : ActionToolBase<ScoringPlane>(type, name, parent)
  {
    declareProperty("Plane", m_config.plane);
    declareProperty("PKill", m_config.pkill);
    declareProperty("FName", m_config.fname);
  }

  std::unique_ptr<ScoringPlane> ScoringPlaneTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<ScoringPlane>(m_config);
  }
  StatusCode ScoringPlaneTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
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
    return ActionToolBase<ScoringPlane>::queryInterface(riid, ppvIf);
  }

} // namespace G4UA
