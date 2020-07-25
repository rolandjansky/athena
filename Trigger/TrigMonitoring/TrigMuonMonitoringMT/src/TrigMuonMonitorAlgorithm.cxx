/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonMonitorAlgorithm.h"


TrigMuonMonitorAlgorithm ::  TrigMuonMonitorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator)
  : AthMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode TrigMuonMonitorAlgorithm :: initialize(){
  StatusCode sc = AthMonitorAlgorithm::initialize();
  ATH_CHECK( m_matchTool.retrieve() );
  ATH_CHECK( m_MuonContainerKey.initialize() );
  return sc;
}


StatusCode TrigMuonMonitorAlgorithm :: fillHistograms(const EventContext& ctx) const {


  bool passToMonitor = selectEvents();

  if(passToMonitor){

    //// Overall monitoring ////
    ATH_CHECK(fillVariables(ctx));

    //// Per chain monitoring ////
    for(const std::string& chain : m_monitored_chains){
      ATH_CHECK( fillVariablesPerChain(ctx, chain) );
    }


    //// Per offline muon monitoring ////
    SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
    if (! muons.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
      return StatusCode::FAILURE;
    }
    std::vector<const xAOD::Muon*> probes;
    ATH_CHECK(selectMuons(muons, probes));

    for(const xAOD::Muon* mu : probes){
      ATH_CHECK( fillVariablesPerOfflineMuon(ctx, mu) );

      //// Per offline muon per chain monitoring ////
      for(const std::string& chain : m_monitored_chains){
	ATH_CHECK( fillVariablesPerOfflineMuonPerChain(ctx, mu, chain) );
      }
    }

  }



  return StatusCode::SUCCESS;
}



bool TrigMuonMonitorAlgorithm :: selectEvents() const {
  return true;
}


StatusCode TrigMuonMonitorAlgorithm :: selectMuons(SG::ReadHandle<xAOD::MuonContainer> &muons, std::vector<const xAOD::Muon*> &probes) const {
  for (const xAOD::Muon* mu : *muons) {
    if(mu->muonType()<=m_muontype){
      probes.push_back(mu);
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigMuonMonitorAlgorithm :: fillVariables(const EventContext& ) const {
  return StatusCode::SUCCESS;
}


StatusCode TrigMuonMonitorAlgorithm :: fillVariablesPerOfflineMuon(const EventContext&, const xAOD::Muon* ) const {
  return StatusCode::SUCCESS;
}

StatusCode TrigMuonMonitorAlgorithm :: fillVariablesPerChain(const EventContext&, const std::string&) const {
  return StatusCode::SUCCESS;
}

StatusCode TrigMuonMonitorAlgorithm :: fillVariablesPerOfflineMuonPerChain(const EventContext&, const xAOD::Muon* , const std::string&) const {
  return StatusCode::SUCCESS;
}
