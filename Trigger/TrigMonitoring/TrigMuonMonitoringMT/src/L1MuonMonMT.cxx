/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1MuonMonMT.h"


L1MuonMonMT :: L1MuonMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode L1MuonMonMT :: initialize(){
  StatusCode sc = TrigMuonMonitorAlgorithm::initialize();
  ATH_CHECK( m_MuonRoIContainerKey.initialize() );
  return sc;
}


StatusCode L1MuonMonMT :: fillVariables(const EventContext &ctx) const {

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

  for (const auto& l1mu : *rois) {
    l1Thres = (float)l1mu->getThrNumber();
    l1Eta = l1mu->eta();
    l1Phi = l1mu->phi();
    fill("L1MuonMonMT", l1Thres, l1Eta, l1Phi);
  }


  return StatusCode::SUCCESS;
}

