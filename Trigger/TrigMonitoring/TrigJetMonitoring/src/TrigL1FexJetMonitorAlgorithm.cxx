/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL1FexJetMonitorAlgorithm.h"
#include "TrigL1FexJetMonitorTool.h"

#include "AsgDataHandles/ReadDecorHandle.h"

#include <vector>

TrigL1FexJetMonitorAlgorithm::TrigL1FexJetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator){
}

TrigL1FexJetMonitorAlgorithm::~TrigL1FexJetMonitorAlgorithm() {}

StatusCode TrigL1FexJetMonitorAlgorithm::initialize() {
  return   AthMonitorAlgorithm::initialize();
}


StatusCode TrigL1FexJetMonitorAlgorithm::fillHistograms(const EventContext& ctx) const {

  for (const auto& filler : m_fillers) {
    std::vector<JetData> jetData;
    
    auto sc = filler->getData(ctx,
			      jetData);
    if (sc.isFailure()) {
      return sc;
    }

    auto groupTool = getGroup(filler->groupName());
    
    for (const auto& jd : jetData) {
      Monitored::Scalar<float> et{"et", jd.m_et};
      Monitored::Scalar<float> eta{"eta", jd.m_eta};
      Monitored::Scalar<float> phi{"phi", jd.m_phi};

      // fill dies not return a StatusCode object
      fill(groupTool, et, eta, phi);
    }

    /*
    if (!m_hltMatchedData.empty()){
      auto sc = fill(groupTool,
		     m_hltMatchedData[TrigJetMonitoringVarEnum::dpt],
		     m_hltMatchedData[TrigJetMonitoringVarEnum::dEnergy],
		     m_hltMatchedData[TrigJetMonitoringVarEnum::dMass],
		     m_hltMatchedData[TrigJetMonitoringVarEnum::rPt],
		     m_hltMatchedData[TrigJetMonitoringVarEnum::rEnergy],
		     m_hltMatchedData[TrigJetMonitoringVarEnum::rMass],
		     m_hltMatchedData[TrigJetMonitoringVarEnum::ptRef],
		     m_hltMatchedData[TrigJetMonitoringVarEnum::etaRef]);
      
      if (sc.failure()) {
	ATH_ERR_MSG("Error filling historgrams for hlt matched jets");
	return  sc;
      }
    }

		  
        
    if (!m_offMatchedData.empty()){
      auto sc = fill(groupTool,
		     m_offMatchedData[TrigJetMonitoringVarEnum::dpt],
		     m_offMatchedData[TrigJetMonitoringVarEnum::dEnergy],
		     m_offMatchedData[TrigJetMonitoringVarEnum::dMass],
		     m_offMatchedData[TrigJetMonitoringVarEnum::rPt],
		     m_offMatchedData[TrigJetMonitoringVarEnum::rEnergy],
		     m_offMatchedData[TrigJetMonitoringVarEnum::rMass],
		     m_offMatchedData[TrigJetMonitoringVarEnum::ptRef],
		     m_offMatchedData[TrigJetMonitoringVarEnum::etaRef]);

      if (sc.failure()) {
	ATH_ERR_MSG("Error filling historgrams for offline matched jets");
	return  sc;
      }
    }
    */
  }
  return StatusCode::SUCCESS;
}

