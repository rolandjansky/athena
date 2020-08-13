/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuZTPMonMT.h"
#include "MuonMatchingTool.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include <algorithm>

TrigMuZTPMonMT :: TrigMuZTPMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode TrigMuZTPMonMT :: initialize(){
  StatusCode sc = TrigMuonMonitorAlgorithm::initialize();

  // Pairing HLT and L1
  unsigned int nchains = m_monitored_chains.size();
  if(nchains!=m_monitored_chains_L1.size() || nchains!=m_monitored_chains_threshold.size()){
    ATH_MSG_ERROR("Configuration seems to be wrong. The size of \"L1Seeds\" and \"Thresholds\" should be same as \"MonitoredChainds\".");
    return StatusCode::FAILURE;
  } else {
    for(unsigned int ichain=0; ichain<nchains; ++ichain){
      m_l1seeds[m_monitored_chains[ichain]] = m_monitored_chains_L1[ichain];
      m_thresholds[m_monitored_chains[ichain]] = m_monitored_chains_threshold[ichain];
    }
  }
  return sc;
}


bool TrigMuZTPMonMT :: selectEvents() const {
  if(m_event_trigger.empty()) return true;
  return getTrigDecisionTool()->isPassed(m_event_trigger);
}



StatusCode TrigMuZTPMonMT :: fillVariablesPerOfflineMuonPerChain(const EventContext& ctx, const xAOD::Muon* mu, const std::string &chain) const {


  auto muPt = Monitored::Scalar<double>(m_group+"_muPt");
  auto muEta = Monitored::Scalar<float>(m_group+"_muEta");
  auto muPhi = Monitored::Scalar<float>(m_group+"_muPhi");

  muPt = mu->pt()/1e3;
  muEta = mu->eta();
  muPhi = mu->phi();

  auto isEndcap = Monitored::Scalar<bool>(m_group+"_isEndcap",false);
  auto isBarrel = Monitored::Scalar<bool>(m_group+"_isBarrel",false);

  if( std::abs(muEta) > 1.05 ) isEndcap = true;
  else isBarrel = true;


  std::vector<int> vec_muRegion;
  auto muRegion = Monitored::Collection(m_group+"_muRegion",vec_muRegion);

  vec_muRegion.clear();

  if(isBarrel) vec_muRegion.push_back(0);
  if(isEndcap) vec_muRegion.push_back(1);

  auto L1pass = Monitored::Scalar<bool>(m_group+"_L1pass",false);
  auto L2SApass = Monitored::Scalar<bool>(m_group+"_L2SApass",false);
  auto L2CBpass = Monitored::Scalar<bool>(m_group+"_L2CBpass",false);
  auto EFSApass = Monitored::Scalar<bool>(m_group+"_EFSApass",false);
  auto EFCBpass = Monitored::Scalar<bool>(m_group+"_EFCBpass",false);
  auto EFIsopass = Monitored::Scalar<bool>(m_group+"_EFIsopass",false);


  if(m_doL1){
    bool activestate = false;
    m_matchTool->matchL1(mu, ctx, m_l1seeds.at(chain), activestate);
    L1pass = activestate;
  } else {
    L1pass = true;
  }


  if(L1pass){
    if(m_doL2SA){
      bool activestate = false;
      m_matchTool->matchL2SA(mu, chain, activestate);
      L2SApass = activestate;
    } else {
      L2SApass = true;
    }
  }


  if(L2SApass){
    if(m_doL2CB){
      bool activestate = false;
      m_matchTool->matchL2CB(mu, chain, activestate);
      L2CBpass = activestate;
    } else {
      L2CBpass = true;
    }
  }


  if(L2CBpass){
    if(m_doEF){
      bool activestate = false;
      m_matchTool->matchEFSA(mu, chain, activestate);
      EFSApass = activestate;
    } else {
      EFSApass = true;
    }
  }


  if(EFSApass){
    if(m_doEF){
      bool activestate = false;
      m_matchTool->matchEFCB(mu, chain, activestate);
      EFCBpass = activestate;
    } else {
      EFCBpass = true;
    }
  }


  if(EFCBpass){
    if(m_doEFIso){
      bool activestate = false;
      m_matchTool->matchEFIso(mu, chain, activestate);
      EFIsopass = activestate;
    } else {
      EFIsopass = true;
    }
  }

  if(m_doEFIso) ATH_MSG_DEBUG("L1pass:" << L1pass << " L2SAPass:" << L2SApass << " L2CBpass:" << L2CBpass << " EFSApass:" << EFSApass <<  " EFCBpass:" << EFCBpass << ", EFIsopass:" << EFIsopass);
  else ATH_MSG_DEBUG("L1pass:" << L1pass << " L2SAPass:" << L2SApass << " L2CBpass:" << L2CBpass << " EFSApass:" << EFSApass <<  " EFCBpass:" << EFCBpass);


  //// Cuts based on the offline muon's features ////
  // Inclusive
  fill(m_group, muPt, L1pass, L2SApass, L2CBpass, EFSApass, EFCBpass, EFIsopass, isEndcap, isBarrel, muRegion);

  // Plateau
  if(muPt>m_thresholds.at(chain)){
    fill(m_group, muEta, muPhi, L1pass, L2SApass, L2CBpass, EFSApass, EFCBpass, EFIsopass, isEndcap, isBarrel, muRegion);
  }




  return StatusCode::SUCCESS;
}



StatusCode TrigMuZTPMonMT :: selectMuons(SG::ReadHandle<xAOD::MuonContainer> &muons, std::vector<const xAOD::Muon*> &probes) const {

  std::vector<float> vec_invmass;
  vec_invmass.clear();

  std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > dimuons;
  xAOD::MuonContainer::const_iterator mu1_it = muons->begin();
  xAOD::MuonContainer::const_iterator mu1_end = muons->end();
  for(; mu1_it!=mu1_end; ++mu1_it){
    const xAOD::Muon *mu1 = *mu1_it;
    if( mu1->muonType()>m_muontype ) continue;
    xAOD::MuonContainer::const_iterator mu2_it = mu1_it;
    xAOD::MuonContainer::const_iterator mu2_end = mu1_end;
    for(++mu2_it; mu2_it!=mu2_end; ++mu2_it){
      const xAOD::Muon *mu2 = *mu2_it;
      if( mu2->muonType()>m_muontype ) continue;
      if( mu1->charge()*mu2->charge()>0 ) continue;

      TLorentzVector lvmu1 = mu1->p4();
      TLorentzVector lvmu2 = mu2->p4();
      double dimu_mass = (lvmu1+lvmu2).M()/1.e3; // convert to GeV
      vec_invmass.push_back(dimu_mass);
      bool bit_mass = (fabs(dimu_mass - 91.1876) < 12.0000);

      // For Z->mumu events
      if( !bit_mass ) continue;
      
      dimuons.push_back(std::make_pair(mu1,mu2));
    }
  }// loop over muons
  
  auto invmass = Monitored::Collection(m_group+"_invmass", vec_invmass);
  fill(m_group, invmass);


  for (std::pair<const xAOD::Muon*,const xAOD::Muon*> dimu : dimuons){
    bool pass1 = false;
    bool pass2 = false;

    m_matchTool->matchEFCB(dimu.first, m_tag_trig, pass1);
    m_matchTool->matchEFCB(dimu.second, m_tag_trig, pass2);
    
    if(pass1){
      if(std::find(probes.begin(), probes.end(), dimu.second)==probes.end()){
	probes.push_back(dimu.second);
      }
    }
    if(pass2){
      if(std::find(probes.begin(), probes.end(), dimu.first)==probes.end()){
	probes.push_back(dimu.first);
      }
    }
  }
  
  return StatusCode::SUCCESS;;
}
