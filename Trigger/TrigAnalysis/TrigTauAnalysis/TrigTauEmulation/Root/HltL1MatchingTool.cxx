/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#include "TrigTauEmulation/HltL1MatchingTool.h"
#include "TrigTauEmulation/Utils.h"
#include "TrigTauEmulation/MsgStream.h"

HltL1MatchingTool::HltL1MatchingTool(const std::string & name) : asg::AsgTool(name)  {
  std::vector<std::string> l1seeds;
  declareProperty("dR_cut", m_dr_cut=0.4, "DR cut");
  declareProperty("L1seeds", m_l1seeds = l1seeds, "L1 seeds");
}


HltL1MatchingTool::HltL1MatchingTool(const HltL1MatchingTool & other) : asg::AsgTool(other.name() + "_copy") {
  m_dr_cut = other.m_dr_cut;
  m_l1seeds = other.m_l1seeds;
} 

StatusCode HltL1MatchingTool::initialize() {
  m_l1seeds.push_back(""); //generic flag; this was the old matching style (best dR)
  return StatusCode::SUCCESS;
}


StatusCode HltL1MatchingTool::execute(const std::vector<DecoratedHltTau>& hlt_taus, const xAOD::EmTauRoIContainer * l1_taus) {
  for (const auto& tau: hlt_taus) {
    ATH_CHECK(match(tau.getHltTau(), l1_taus));
  }

  return StatusCode::SUCCESS;
}

StatusCode HltL1MatchingTool::execute(const xAOD::TauJetContainer * hlt_taus, const xAOD::EmTauRoIContainer * l1_taus) {
  for (const auto tau: *hlt_taus) {
    ATH_CHECK(match(tau, l1_taus));
  }

  return StatusCode::SUCCESS;
}


StatusCode HltL1MatchingTool::match(const xAOD::TauJet *hlt_tau, const xAOD::EmTauRoIContainer * l1_taus) {

  //std::set<std::string> seeds(m_l1seeds);
  
  MY_MSG_DEBUG("Using seeds: ");
  for (auto s: m_l1seeds) { 
      if(s == "") { 
        MY_MSG_DEBUG("\t<empty string>"); 
        hlt_tau->auxdecor<int>("l1_index") = -1;
      } else { 
        MY_MSG_DEBUG("\t" << s); 
        hlt_tau->auxdecor<int>("l1_index_" + s) = -1;
      }
  }

  for(auto s: m_l1seeds){
    double dR_init = 9999.;
    std::string idx_name = "l1_index";
    std::string seed_name = "<empty string>";
    if(s != "") { idx_name += "_" + s; seed_name = s; }
    
    for (const auto l1_tau: *l1_taus) {
      // is it a tau? 
      if (l1_tau->roiType() != xAOD::EmTauRoI::TauRoIWord) {
        MY_MSG_DEBUG("seed " << seed_name << ": skipping l1_index " << l1_tau->index() << " due to non-matching roiType");
        continue;
      }
      
      // if we use the seed - did the L1 pass?
      if (s != "") { 
        MY_MSG_DEBUG("seed " << seed_name << ": l1_index " << l1_tau->index() << " L1 passed = " << l1_tau->auxdataConst<bool>(s)); 
      }
      
      // find the closest in dR
      double dR = Utils::DeltaR(hlt_tau->eta(), hlt_tau->phi(), l1_tau->eta(), l1_tau->phi());
      if (dR < m_dr_cut && dR < dR_init and (s == "" or l1_tau->auxdataConst<bool>(s)) ) {
        dR_init = dR;
        MY_MSG_DEBUG("seed " << seed_name << ": matched: dR = "<< dR << " for l1_index " << l1_tau->index() << " at " << idx_name);
        hlt_tau->auxdecor<int>(idx_name) = l1_tau->index();
        // break;
      }
    }
  }
  return StatusCode::SUCCESS;
}
