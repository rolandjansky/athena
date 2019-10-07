/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonMonitorAlgorithm.h"


TrigMuonMonitorAlgorithm ::  TrigMuonMonitorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator)
  : AthMonitorAlgorithm(name, pSvcLocator),
    m_MuonContainerKey("Muons")
{}


StatusCode TrigMuonMonitorAlgorithm :: initialize(){
  StatusCode sc = AthMonitorAlgorithm::initialize();
  ATH_CHECK( m_MuonContainerKey.initialize() );
  return sc;
}


StatusCode TrigMuonMonitorAlgorithm :: fillHistograms(const EventContext& ctx) const {


  bool passToMonitor = selectEvents();

  if(passToMonitor){

    //// Overall monitoring ////
    ATH_CHECK(fillVariables(ctx));


    //// Per offline muon monitoring ////
    SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
    if (! muons.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
      return StatusCode::FAILURE;
    }
    std::vector<const xAOD::Muon*> probes;
    selectMuons(muons, probes);

    for(const xAOD::Muon* mu : probes){
      fillVariablesPerOfflineMuon(ctx, mu);
    }

  }



  return StatusCode::SUCCESS;
}



bool TrigMuonMonitorAlgorithm :: selectEvents() const {
  return true;
}


bool TrigMuonMonitorAlgorithm :: selectMuons(SG::ReadHandle<xAOD::MuonContainer> &muons, std::vector<const xAOD::Muon*> &probes) const {
  for (const xAOD::Muon* mu : *muons) {
    probes.push_back(mu);
  }
  return true;
}


StatusCode TrigMuonMonitorAlgorithm :: fillVariables(const EventContext& ) const {
  return StatusCode::SUCCESS;
}


StatusCode TrigMuonMonitorAlgorithm :: fillVariablesPerOfflineMuon(const EventContext&, const xAOD::Muon* ) const {
  return StatusCode::SUCCESS;
}
