/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "L2MuonSAMonMT.h"


L2MuonSAMonMT :: L2MuonSAMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode L2MuonSAMonMT :: initialize(){
  StatusCode sc = TrigMuonMonitorAlgorithm::initialize();
  ATH_CHECK( m_L2MuonSAContainerKey.initialize() );
  return sc;
}


StatusCode L2MuonSAMonMT :: fillVariables(const EventContext &ctx) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  SG::ReadHandle<xAOD::L2StandAloneMuonContainer> muonSAs(m_L2MuonSAContainerKey, ctx);
  if (! muonSAs.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain xAOD::L2StandAloneMuon collection with name "<< m_L2MuonSAContainerKey);
    return StatusCode::FAILURE;
  }

  // variables
  auto saPt = Monitored::Scalar<double>("saPt",-1.0);
  auto saEta = Monitored::Scalar<double>("saEta",-1.0);
  auto saPhi = Monitored::Scalar<double>("saPhi",-1.0);

  for (const auto& l2sa : *muonSAs) {
    saPt = l2sa->pt();
    saEta = l2sa->eta();
    saPhi = l2sa->phi();
    fill("L2MuonSAMonMT", saPt, saEta, saPhi);
  }


  return StatusCode::SUCCESS;
}

