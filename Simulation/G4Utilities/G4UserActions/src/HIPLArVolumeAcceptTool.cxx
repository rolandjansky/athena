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
  {
    declareInterface<ISteppingActionTool>(this);
    declareInterface<IBeginEventActionTool>(this);
    declareInterface<IEndEventActionTool>(this);
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<HIPLArVolumeAccept> HIPLArVolumeAcceptTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<HIPLArVolumeAccept>();
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
