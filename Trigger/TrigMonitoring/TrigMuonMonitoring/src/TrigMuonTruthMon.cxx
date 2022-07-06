/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile TrigMuonTruthMon.cxx
 *   
 *     author: Harriet Watson (hawatson@cern.ch)
 *       date: 14.04.2021
 * 
 *   Class to monitor the muon trigger truth efficiency for each trigger chain. 
 *   Produces histograms of truth muons and truth muons which pass the trigger.
 *   
 */   

#include "TrigMuonTruthMon.h"

TrigMuonTruthMon :: TrigMuonTruthMon(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}

StatusCode TrigMuonTruthMon :: initialize(){
  StatusCode sc = TrigMuonMonitorAlgorithm::initialize();
  ATH_CHECK( m_muonTruthParticlesKey.initialize() );
  return sc;
}

StatusCode TrigMuonTruthMon :: fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const {
  ATH_MSG_DEBUG("Filling muon truth monitoring histograms for " << name() << "...");

  SG::ReadHandle<xAOD::TruthParticleContainer> truthMuons(m_muonTruthParticlesKey, ctx); // Access truth muons
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_EventInfoKey, ctx);                        // Access event info
  
  if (! truthMuons.isValid() ) {
    ATH_MSG_ERROR("xAOD::TruthParticleContainer with name "<< m_muonTruthParticlesKey << " is not valid");
    return StatusCode::FAILURE;
  }

  auto truthEta = Monitored::Scalar<float>(chain+"_truthEta",-999.);
  auto truthPhi = Monitored::Scalar<float>(chain+"_truthPhi",-999.);
  auto truthPt = Monitored::Scalar<float>(chain+"_truthPt",-999.);
  auto truthEndcapPt = Monitored::Scalar<float>(chain+"_truthEndcapPt",-999.);
  auto truthBarrelPt = Monitored::Scalar<float>(chain+"_truthBarrelPt",-999.);
  auto truthIntPerBC = Monitored::Scalar<float>(chain+"_truthIntPerBC",-999.);

  auto MatchedEFCBtruthEta = Monitored::Scalar<float>(chain+"_MatchedEFCBtruthEta",-999.); // Names need to match those in the config.py file
  auto MatchedEFCBtruthPhi = Monitored::Scalar<float>(chain+"_MatchedEFCBtruthPhi",-999.);
  auto MatchedEFCBtruthPt = Monitored::Scalar<float>(chain+"_MatchedEFCBtruthPt",-999.);
  auto MatchedEFCBtruthEndcapPt = Monitored::Scalar<float>(chain+"_MatchedEFCBtruthEndcapPt",-999.);
  auto MatchedEFCBtruthBarrelPt = Monitored::Scalar<float>(chain+"_MatchedEFCBtruthBarrelPt",-999.);
  auto MatchedEFCBtruthIntPerBC = Monitored::Scalar<float>(chain+"_MatchedEFCBtruthIntPerBC",-999.);
  auto MatchedL1truthEta = Monitored::Scalar<float>(chain+"_MatchedL1truthEta",-999.); // Names need to match those in the config.py file
  auto MatchedL1truthPhi = Monitored::Scalar<float>(chain+"_MatchedL1truthPhi",-999.);
  auto MatchedL1truthPt = Monitored::Scalar<float>(chain+"_MatchedL1truthPt",-999.);
  auto MatchedL1truthEndcapPt = Monitored::Scalar<float>(chain+"_MatchedL1truthEndcapPt",-999.);
  auto MatchedL1truthBarrelPt = Monitored::Scalar<float>(chain+"_MatchedL1truthBarrelPt",-999.);
  auto MatchedL1truthIntPerBC = Monitored::Scalar<float>(chain+"_MatchedL1truthIntPerBC",-999.);

  bool passed_EF = false;
  bool passed_L1 = false;

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
  pT_cut = pT_cut + 1.0;

  for (const auto truthMu : *truthMuons) {

    // Fill truth histograms
    double eta = 0.0; // Check eta to split pT into endcap barrel
    eta = truthMu->eta();
    if(std::abs(eta) > 2.5) continue; // cut on eta to only fill with muons inside detector geometry
    
    truthPt = truthMu->pt()/1e3;
    fill(m_group+"_"+chain,truthPt);
    if(std::abs(eta) < 1.05){ 
      truthBarrelPt = truthMu->pt()/1e3;
      fill(m_group+"_"+chain, truthBarrelPt);
    }
    else{ // 1.05 < |eta| < 2.5 
      truthEndcapPt = truthMu->pt()/1e3;
      fill(m_group+"_"+chain, truthEndcapPt); 
    }
    if (pT_cut < truthPt){ // Apply pT cut to eta and phi distributions only
      truthEta = truthMu->eta();
      truthPhi = truthMu->phi();
      truthIntPerBC = eventInfo->actualInteractionsPerCrossing();
      fill(m_group+"_"+chain, truthEta, truthPhi, truthIntPerBC);
    }

    // Find match truth muons - EFSA matching for msonly chains, otherwise EFCB matching
    std::string msonly = "msonly";
    const xAOD::Muon* efmuon;
    if(chain.find(msonly) != std::string::npos){ // Find EFCB muons
      efmuon = m_matchTool->matchEFSA(truthMu, chain, passed_EF);
    }
    else{ // Find EFCB muons
      efmuon = m_matchTool->matchEFCB(truthMu, chain, passed_EF);  
    }
    const xAOD::MuonRoI* l1muon = m_matchTool->matchL1(truthMu, chain, passed_L1);    // Find L1 muons

    if(efmuon && passed_EF){  // Fill matched muon histograms
      MatchedEFCBtruthPt = truthMu->pt()/1e3;
      fill(m_group+"_"+chain, MatchedEFCBtruthPt);
      if(std::abs(eta) < 1.05){ 
        MatchedEFCBtruthBarrelPt = truthMu->pt()/1e3;
        fill(m_group+"_"+chain, MatchedEFCBtruthBarrelPt);
      }
      else{ // 1.05 < |eta| < 2.5 
        MatchedEFCBtruthEndcapPt = truthMu->pt()/1e3;
        fill(m_group+"_"+chain, MatchedEFCBtruthEndcapPt);
      } 
      if (pT_cut < truthPt){ 
        MatchedEFCBtruthEta = truthMu->eta();
        MatchedEFCBtruthPhi = truthMu->phi();
        MatchedEFCBtruthIntPerBC = eventInfo->actualInteractionsPerCrossing();
        fill(m_group+"_"+chain, MatchedEFCBtruthEta, MatchedEFCBtruthPhi, MatchedEFCBtruthIntPerBC);
      }
    }
    
    if(l1muon && passed_L1){ // Fill L1 muon matched histograms
      MatchedL1truthPt = truthMu->pt()/1e3;
      fill(m_group+"_"+chain, MatchedL1truthPt);
      if(std::abs(eta) < 1.05){ 
        MatchedL1truthBarrelPt = truthMu->pt()/1e3;
        fill(m_group+"_"+chain, MatchedL1truthBarrelPt);
      }
      else{ // 1.05 < |eta| < 2.5 
        MatchedL1truthEndcapPt = truthMu->pt()/1e3;
        fill(m_group+"_"+chain, MatchedL1truthEndcapPt);
      } 
      if (pT_cut < truthPt){ 
        MatchedL1truthEta = truthMu->eta();
        MatchedL1truthPhi = truthMu->phi();
        MatchedL1truthIntPerBC = eventInfo->actualInteractionsPerCrossing();
        fill(m_group+"_"+chain, MatchedL1truthEta, MatchedL1truthPhi, MatchedL1truthIntPerBC);
      }
    }
    
    if((l1muon && passed_L1) && !(passed_EF && efmuon)){
      ATH_MSG_DEBUG("MuonTruthMon: passed L1 but not HLT");
    }

    if(!(passed_L1 && l1muon) && (passed_EF && efmuon)){
      ATH_MSG_DEBUG("MuonTruthMon: passed HLT but not L1");
    }

    passed_L1 = false;
    passed_EF = false;
    
  }

  return StatusCode::SUCCESS;
}
