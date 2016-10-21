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
    if(riid == IBeginRunActionTool::interfaceID()) {
      *ppvIf = (IBeginRunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<GeantFollower>::queryInterface(riid, ppvIf);
  }

} // namespace G4UA
