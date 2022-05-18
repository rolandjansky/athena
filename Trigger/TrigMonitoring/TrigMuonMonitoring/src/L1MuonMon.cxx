/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "L1MuonMon.h"


L1MuonMon :: L1MuonMon(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode L1MuonMon :: initialize(){
  StatusCode sc = TrigMuonMonitorAlgorithm::initialize();
  ATH_CHECK( m_MuonRoIContainerKey.initialize() );
  return sc;
}


StatusCode L1MuonMon :: fillVariables(const EventContext &ctx) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  SG::ReadHandle<xAOD::MuonRoIContainer> rois(m_MuonRoIContainerKey, ctx);
  if (! rois.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain xAOD::MuonRoI collection with name "<< m_MuonRoIContainerKey);
    return StatusCode::FAILURE;
  }

  // variables
  auto l1Thres = Monitored::Scalar<double>("l1Thres",-1.0);
  auto l1Eta = Monitored::Scalar<double>("l1Eta",-1.0);
  auto l1Phi = Monitored::Scalar<double>("l1Phi",-1.0);

  for (const auto l1mu : *rois) {
    l1Thres = (float)l1mu->getThrNumber();
    l1Eta = l1mu->eta();
    l1Phi = l1mu->phi();
    fill("L1MuonMon", l1Thres, l1Eta, l1Phi);
  }


  return StatusCode::SUCCESS;
}

