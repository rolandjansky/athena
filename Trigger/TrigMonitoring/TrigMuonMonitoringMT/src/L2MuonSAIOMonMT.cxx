/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L2MuonSAIOMonMT.h"

#include "xAODTrigMuon/TrigMuonDefs.h"
#include "MuonMatchingTool.h"

L2MuonSAIOMonMT :: L2MuonSAIOMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}

StatusCode L2MuonSAIOMonMT :: initialize(){

  ATH_CHECK( TrigMuonMonitorAlgorithm::initialize() );

  unsigned int nchains = m_monitored_chains.size();
  if(nchains!=m_monitored_chains_plateau.size()){
    ATH_MSG_ERROR("Configuration seems to be wrong. The size of \"Plateaus\" should be same as \"MonitoredChainds\".");
    return StatusCode::FAILURE;
  } else {
    for(unsigned int ichain=0; ichain<nchains; ++ichain){
      m_plateaus[m_monitored_chains[ichain]] = m_monitored_chains_plateau[ichain] + 2.;
    }
  }
  return StatusCode::SUCCESS;
}



StatusCode L2MuonSAIOMonMT :: fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  auto passIOchain = Monitored::Scalar<bool>(chain+"_passIOchain",false);
  auto OffdR = Monitored::Scalar<float>(chain+"_OffdR",1000.);

  if( getTrigDecisionTool()->isPassed(chain) ) passIOchain = true;

  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
  if (! muons.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    return StatusCode::FAILURE;
  }

  xAOD::MuonContainer::const_iterator mu1_it = muons->begin();
  xAOD::MuonContainer::const_iterator mu1_end = muons->end();
  for(; mu1_it!=mu1_end; ++mu1_it){
    const xAOD::Muon *mu1 = *mu1_it;
    if( mu1->muonType()>m_muontype ) continue;
    if( mu1->pt()/1e3 < m_plateaus.at(chain) ) continue;
    xAOD::MuonContainer::const_iterator mu2_it = mu1_it;
    xAOD::MuonContainer::const_iterator mu2_end = mu1_end;
    for(++mu2_it; mu2_it!=mu2_end; ++mu2_it){
      const xAOD::Muon *mu2 = *mu2_it;
      if( mu2->muonType()>m_muontype ) continue;
      if( mu2->pt()/1e3 < m_plateaus.at(chain) ) continue;
      if( mu1->charge()*mu2->charge()>0 ) continue;

      double dR = xAOD::P4Helpers::deltaR(mu1, mu2);
      if(dR < OffdR) OffdR = dR;
    }
  }

  fill(m_group+"_"+chain, passIOchain, OffdR);

  return StatusCode::SUCCESS;
}
