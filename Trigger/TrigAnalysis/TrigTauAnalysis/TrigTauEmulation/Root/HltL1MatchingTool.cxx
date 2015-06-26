/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#include "TrigTauEmulation/HltL1MatchingTool.h"
#include "TrigTauEmulation/Utils.h"

HltL1MatchingTool::HltL1MatchingTool(const std::string & name) : asg::AsgTool(name) 

{

  std::vector<std::string> l1seeds;
  declareProperty("dR_cut", m_dr_cut=0.4, "DR cut");
  declareProperty("L1seeds", m_l1seeds = l1seeds, "L1 seeds");
}


HltL1MatchingTool::HltL1MatchingTool(const HltL1MatchingTool & other) : asg::AsgTool(other.name() + "_copy")
{

} 

StatusCode HltL1MatchingTool::initialize()
{
  return StatusCode::SUCCESS;
}



StatusCode HltL1MatchingTool::execute(const xAOD::TauJetContainer * hlt_taus, const xAOD::EmTauRoIContainer * l1_taus)

{
  for (const auto tau: *hlt_taus)
    match(tau, l1_taus);

  return StatusCode::SUCCESS;
}


StatusCode HltL1MatchingTool::match(const xAOD::TauJet * hlt_tau, const xAOD::EmTauRoIContainer * l1_taus) {
  hlt_tau->auxdecor<int>("l1_index") = -1;

  std::vector<std::string> seeds(m_l1seeds);
  seeds.push_back(""); //generic flag; this was the old matching style (best dR)

  for(auto s: seeds){
    double dR_init = 9999.;
    for (const auto l1_tau: *l1_taus) {
      std::string idx_name = "l1_index";
      if(s != "") { idx_name += "_" + s; }
      if (l1_tau->roiType() != xAOD::EmTauRoI::TauRoIWord) {
        continue;
      }

      double dR = Utils::DeltaR(hlt_tau->eta(), hlt_tau->phi(), l1_tau->eta(), l1_tau->phi());
      if (dR < m_dr_cut && dR < dR_init and (s == "" or l1_tau->auxdataConst<bool>(s)) ) {
        dR_init = dR;
        ATH_MSG_DEBUG("Matched: dR = "<< dR << " for seed '" << s << "'");
        hlt_tau->auxdecor<int>(idx_name) = l1_tau->index();
        // break;
      }
    }
  }
  return StatusCode::SUCCESS;
}
