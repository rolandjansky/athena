/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigAFPSidHypoMonitoringAlg.h"

#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrack.h"

TrigAFPSidHypoMonitoringAlg::TrigAFPSidHypoMonitoringAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthMonitorAlgorithm (name, pSvcLocator)
{
}

TrigAFPSidHypoMonitoringAlg::~TrigAFPSidHypoMonitoringAlg()
{
}

StatusCode TrigAFPSidHypoMonitoringAlg::initialize()
{
  ATH_CHECK(AthMonitorAlgorithm::initialize());
 
  ATH_CHECK(m_AFPtrackKey.initialize());
  ATH_CHECK(m_AFPtrackOffKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode TrigAFPSidHypoMonitoringAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TrigAFPSidHypoMonitoringAlg::fillHistograms(const EventContext& context) const
{

  SG::ReadHandle<xAOD::AFPTrackContainer> tracksAFP(m_AFPtrackKey, context);

  SG::ReadHandle<xAOD::AFPTrackContainer> tracksAFPoff(m_AFPtrackOffKey, context);

  ATH_MSG_DEBUG("AFP track size online/offline:"<<tracksAFP.get()->size()<<"/"<<tracksAFPoff.get()->size());

  auto xDiff = Monitored::Scalar("xDiff",-999.);
  auto yDiff = Monitored::Scalar("yDiff",-999.);

  float dR = -999.;
  float dRmin = 9e9;

  // Match online track to offline
  for(const auto track: *tracksAFP){
    for(const auto off_track: *tracksAFPoff){
      
      xDiff = track->xLocal()-off_track->xLocal();
      yDiff = track->yLocal()-off_track->yLocal();

      dR = TMath::Sqrt(TMath::Power(xDiff,2)+TMath::Power(yDiff,2));

      if(dR<dRmin){
	dRmin = dR;
      }
    } // End of loop over offline tracks

    fill("AFPCoarse", xDiff, yDiff);

  }// End of loop over online tracks

  // in future should be split into many smaller methods called from here
  std::vector<std::string> passedAFPChains = {"all"}; // also includes ALL events counter
  for (auto chainName: m_chains) {
    if ( getTrigDecisionTool()->isPassed(chainName) ){
      passedAFPChains.emplace_back(chainName);
    }

  }
  if ( passedAFPChains.size() > 1) passedAFPChains.emplace_back("AFP");
  auto whichTrigger = Monitored::Collection("TrigCounts", passedAFPChains);
  fill("AFPCoarse", whichTrigger);

  return StatusCode::SUCCESS;
}

