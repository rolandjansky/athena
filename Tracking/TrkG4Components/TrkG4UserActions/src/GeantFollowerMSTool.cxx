/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrkG4UserActions/GeantFollowerMSTool.h"
#include "TrkG4UserActions/IGeantFollowerMSHelper.h"

namespace G4UA{


  GeantFollowerMSTool::GeantFollowerMSTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<GeantFollowerMS>(type, name, parent), m_config(){
    declareProperty("HelperTool",m_config.helper);
  }

  std::unique_ptr<GeantFollowerMS>  GeantFollowerMSTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<GeantFollowerMS>(m_config);
    return std::move(action);
  }

  StatusCode GeantFollowerMSTool::queryInterface(const InterfaceID& riid, void** ppvIf){

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
    return ActionToolBase<GeantFollowerMS>::queryInterface(riid, ppvIf);
  }

} // namespace G4UA
