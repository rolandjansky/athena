/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4HitFilterTool.h"

namespace G4UA{

  G4HitFilterTool::G4HitFilterTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBaseReport<G4HitFilter>(type, name, parent), m_config(){
    declareProperty("VolumeNames", m_config.volumenames);
  }
  std::unique_ptr<G4HitFilter>  G4HitFilterTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<G4HitFilter>(m_config);
    return std::move(action);
  }
  StatusCode G4HitFilterTool::queryInterface(const InterfaceID& riid, void** ppvIf){

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

    return ActionToolBase<G4HitFilter>::queryInterface(riid, ppvIf);
  }

  StatusCode G4HitFilterTool::finalize(){

    mergeReports();

    ATH_MSG_INFO("processed "<< m_report.ntot <<" events, "<< m_report.npass<<" events passed filter ");
    return StatusCode::SUCCESS;
  }


} // namespace G4UA
