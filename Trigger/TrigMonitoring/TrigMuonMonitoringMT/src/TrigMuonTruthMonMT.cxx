/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile TrigMuonTruthMonMT.cxx
 *   
 *     author: Harriet Watson (hawatson@cern.ch)
 *       date: 14.04.2021
 * 
 *   Class to monitor the muon trigger truth efficiency for each trigger chain. 
 *   Produces histograms of truth muons and truth muons which pass the trigger.
 *   
 */   

#include "TrigMuonTruthMonMT.h"

TrigMuonTruthMonMT :: TrigMuonTruthMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}

StatusCode TrigMuonTruthMonMT :: initialize(){
  StatusCode sc = TrigMuonMonitorAlgorithm::initialize();
  ATH_CHECK( m_muonTruthParticlesKey.initialize() );
  return sc;
}

StatusCode TrigMuonTruthMonMT :: fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const {
  ATH_MSG_DEBUG ("Filling muon truth monitoring histograms for " << name() << "...");

  SG::ReadHandle<xAOD::TruthParticleContainer> truthMuons(m_muonTruthParticlesKey, ctx);
  
  if (! truthMuons.isValid() ) {
    ATH_MSG_ERROR("xAOD::TruthParticleContainer with name "<< m_muonTruthParticlesKey << " is not valid");
    return StatusCode::FAILURE;
  }

  auto truthEta = Monitored::Scalar<float>(chain+"_truthEta",-999.);
  auto truthPhi = Monitored::Scalar<float>(chain+"_truthPhi",-999.);
  auto truthPt = Monitored::Scalar<float>(chain+"_truthPt",-999.);

  auto MatchedEFCBtruthEta = Monitored::Scalar<float>(chain+"_MatchedEFCBtruthEta",-999.); // Names need to match those in the config.py file
  auto MatchedEFCBtruthPhi = Monitored::Scalar<float>(chain+"_MatchedEFCBtruthPhi",-999.);
  auto MatchedEFCBtruthPt = Monitored::Scalar<float>(chain+"_MatchedEFCBtruthPt",-999.);

  auto failHypothesis = Monitored::Scalar<float>(chain+"_failHypothesis",-999.);
  auto failMatch = Monitored::Scalar<float>(chain+"_failMatch",-999.);
  bool passed = false;

  // Find pT cut from chain name
  double pT_cut = 0.0;
  std::size_t index_i = chain.find("mu")+2;
  std::size_t index_f = chain.find("_", index_i+1);
  if (index_f < chain.length()) {
    pT_cut = std::stod(chain.substr(index_i, index_f - index_i));
  }
  else {
    pT_cut = std::stod(chain.substr(index_i, chain.length()-1));
  }

  for (const auto& truthMu : *truthMuons) {

    // Fill truth histograms
    truthPt = truthMu->pt()/1e3;
    fill(m_group+"_"+chain,truthPt);
    if (pT_cut + 1.0 < truthPt){ // Apply pT cut to eta and phi distributions only
      truthEta = truthMu->eta();
      truthPhi = truthMu->phi();
      fill(m_group+"_"+chain, truthEta, truthPhi);
    }

    // Fill matched EFCB 2 truth histograms
    const xAOD::Muon* efmuon = m_matchTool->matchEFCB(truthMu, chain, passed);
    if(efmuon && passed){ 
      MatchedEFCBtruthPt = efmuon->pt()/1e3;
      fill(m_group+"_"+chain, MatchedEFCBtruthPt); 
      if (pT_cut + 1.0 < truthPt){ 
        MatchedEFCBtruthEta = efmuon->eta();
        MatchedEFCBtruthPhi = efmuon->phi();
        fill(m_group+"_"+chain, MatchedEFCBtruthEta, MatchedEFCBtruthPhi);
      }
    }
    else if(!efmuon){ 
      failMatch = 1.0;
      fill(m_group+"_"+chain,failMatch); // Fill to count the number of events that fail the truth matching
    }
    if(!passed){
      failHypothesis = 1.0;
      fill(m_group+"_"+chain,failHypothesis); // Fill to count the number of events that fail the hypothesis requirements
    }
    passed = false;  
  }

  return StatusCode::SUCCESS;
}