/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL1FexJetMonitorAlgorithm.h"
#include "TrigL1FexJetMonitorTool.h"

#include "AsgDataHandles/ReadDecorHandle.h"

#include <vector>
#include <iostream>

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

    std::vector<JetMatchData> jetMatchData;
    sc = filler->getMatchData(ctx,
			      MatchToEnum::hlt,
			      jetMatchData);
    if (sc.isFailure()) {
      return sc;
    }

    
    std::cerr << "PS DEBUG " << name() << " hlt matchdata size "
	      << jetMatchData.size() << '\n';
    
    for (const auto& jd : jetMatchData) {
       auto dPt = Monitored::Scalar("hltptdiff", jd.m_dPt);
       auto dEnergy = Monitored::Scalar("hltenergydiff", jd.m_dEnergy);
       auto dMass = Monitored::Scalar("hltmassdiff", jd.m_dMass);
       auto rPt = Monitored::Scalar("hltptresp", jd.m_rPt);
       auto rEnergy = Monitored::Scalar("hltenergyresp", jd.m_rEnergy);
       auto rMass = Monitored::Scalar("hltmassresp", jd.m_rMass);
       auto ptRef = Monitored::Scalar("hltptref", jd.m_ptRef);
       auto etaRef = Monitored::Scalar("hltetaref", jd.m_etaRef);
       
       // fill dies not return a StatusCode object
       fill(groupTool, dPt, dEnergy, dMass, rPt, rEnergy, rMass, ptRef, etaRef);
    }
    
    

    jetMatchData.clear();
    sc = filler->getMatchData(ctx,
			      MatchToEnum::offline,
			      jetMatchData);
    if (sc.isFailure()) {
      return sc;
    }

    std::cerr << "PS DEBUG " << name() << " offline matchdata size "
	      << jetMatchData.size() << '\n';
    
    for (const auto& jd : jetMatchData) {
       auto dPt = Monitored::Scalar("offptdiff", jd.m_dPt);
       auto dEnergy = Monitored::Scalar("offenergydiff", jd.m_dEnergy);
       auto dMass = Monitored::Scalar("offmassdiff", jd.m_dMass);
       auto rPt = Monitored::Scalar("offptresp", jd.m_rPt);
       auto rEnergy = Monitored::Scalar("offenergyresp", jd.m_rEnergy);
       auto rMass = Monitored::Scalar("offmassresp", jd.m_rMass);
       auto ptRef = Monitored::Scalar("offptref", jd.m_ptRef);
       auto etaRef = Monitored::Scalar("offetaref", jd.m_etaRef);
       
       // fill dies not return a StatusCode object
       fill(groupTool, dPt, dEnergy, dMass, rPt, rEnergy, rMass, ptRef, etaRef);
    }
    
  }
  return StatusCode::SUCCESS;
}

