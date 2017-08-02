/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/HIPLArVolumeAcceptTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  HIPLArVolumeAcceptTool::HIPLArVolumeAcceptTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
    : ActionToolBaseReport<HIPLArVolumeAccept>(type, name, parent)
  {}
  
  //---------------------------------------------------------------------------
  std::unique_ptr<HIPLArVolumeAccept>  HIPLArVolumeAcceptTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<HIPLArVolumeAccept>();
    return std::move(action);
  }

  //---------------------------------------------------------------------------
  StatusCode HIPLArVolumeAcceptTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } if(riid == IBeginEventActionTool::interfaceID()) {
      *ppvIf = (IBeginEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } if(riid == IEndEventActionTool::interfaceID()) {
      *ppvIf = (IEndEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } 

    return ActionToolBase<HIPLArVolumeAccept>::queryInterface(riid, ppvIf);
  }

  //---------------------------------------------------------------------------
  StatusCode HIPLArVolumeAcceptTool::finalize()
  {
    mergeReports();

    ATH_MSG_INFO("#########################################");
    ATH_MSG_INFO("##                                     ##");
    ATH_MSG_INFO( "##    HIPLArVolumeAccept - EndOfRun    ##");
    ATH_MSG_INFO( "##                                     ##");
    ATH_MSG_INFO( "#########################################");
    ATH_MSG_INFO(m_report.HIPevts       <<" events were processed by HIPLArVolumeAccept");
    ATH_MSG_INFO(m_report.HIPevts_failed<<" events were killed because they had no HIP in EMB or EMEC");
    double HIPfraction=1.*(m_report.HIPevts-m_report.HIPevts_failed)/m_report.HIPevts;
    ATH_MSG_INFO("HIP Acceptance: "<<HIPfraction);

    return StatusCode::SUCCESS;
  }

} // namespace G4UA
