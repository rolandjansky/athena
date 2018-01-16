/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrkG4UserActions/GeantFollowerTool.h"

namespace G4UA{


  GeantFollowerTool::GeantFollowerTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<GeantFollower>(type, name, parent), m_config(){
    declareProperty("HelperTool",m_config.helper);
  }

  std::unique_ptr<GeantFollower>  GeantFollowerTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<GeantFollower>(m_config);
    return std::move(action);
  }

  StatusCode GeantFollowerTool::queryInterface(const InterfaceID& riid, void** ppvIf){

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
    return ActionToolBase<GeantFollower>::queryInterface(riid, ppvIf);
  }

} // namespace G4UA
