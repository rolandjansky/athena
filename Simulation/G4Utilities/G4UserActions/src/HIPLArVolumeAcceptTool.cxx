/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/HIPLArVolumeAcceptTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  HIPLArVolumeAcceptTool::HIPLArVolumeAcceptTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
    : UserActionToolBase<HIPLArVolumeAccept>(type, name, parent)
  {
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<HIPLArVolumeAccept>
  HIPLArVolumeAcceptTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Making a HIPLArVolumeAccept action");
    auto action = std::make_unique<HIPLArVolumeAccept>();
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

  //---------------------------------------------------------------------------
  StatusCode HIPLArVolumeAcceptTool::finalize()
  {
    // Accumulate the results across threads
    HIPLArVolumeAccept::Report report;
    m_actions.accumulate(report, &HIPLArVolumeAccept::getReport,
                         &HIPLArVolumeAccept::Report::merge);

    ATH_MSG_INFO("#########################################");
    ATH_MSG_INFO("##                                     ##");
    ATH_MSG_INFO( "##    HIPLArVolumeAccept - EndOfRun    ##");
    ATH_MSG_INFO( "##                                     ##");
    ATH_MSG_INFO( "#########################################");
    ATH_MSG_INFO(report.HIPevts       <<" events were processed by HIPLArVolumeAccept");
    ATH_MSG_INFO(report.HIPevts_failed<<" events were killed because they had no HIP in EMB or EMEC");
    double HIPfraction=1.*(report.HIPevts-report.HIPevts_failed)/report.HIPevts;
    ATH_MSG_INFO("HIP Acceptance: "<<HIPfraction);

    return StatusCode::SUCCESS;
  }

} // namespace G4UA
