/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEfficiencyMonMT.h"
#include "MuonMatchingTool.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include <algorithm>

TrigMuonEfficiencyMonMT :: TrigMuonEfficiencyMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode TrigMuonEfficiencyMonMT :: initialize(){
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


bool TrigMuonEfficiencyMonMT :: selectEvents() const {
  if(m_event_trigger.empty()) return true;
  return getTrigDecisionTool()->isPassed(m_event_trigger);
}



StatusCode TrigMuonEfficiencyMonMT :: selectMuons(SG::ReadHandle<xAOD::MuonContainer> &muons, std::vector<const xAOD::Muon*> &probes) const {

  if(m_eff_method.value().find("TagAndProbe")!=std::string::npos){
    return selectMuonsTagAndProbe(muons, probes);
  } else {
    for (const xAOD::Muon* mu : *muons) {
      if( mu->muonType()<=m_muontype ){
	probes.push_back(mu);
      }
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigMuonEfficiencyMonMT :: fillVariablesPerOfflineMuonPerChain(const EventContext& ctx, const xAOD::Muon* mu, const std::string &chain) const {


  auto muPt = Monitored::Scalar<double>(m_group+"_muPt");
  auto muEta = Monitored::Scalar<float>(m_group+"_muEta");
  auto muPhi = Monitored::Scalar<float>(m_group+"_muPhi");
  auto averageMu = Monitored::Scalar<float>(m_group+"_averageMu");

  muPt = mu->pt()/1e3;
  muEta = mu->eta();
  muPhi = mu->phi();
  averageMu = lbAverageInteractionsPerCrossing(ctx);

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
    if(m_doEFSA){
      bool activestate = false;
      m_matchTool->matchEFSA(mu, chain, activestate);
      EFSApass = activestate;
    } else {
      EFSApass = true;
    }
  }


  if(EFSApass){
    if(m_doEFCB){
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


  ATH_MSG_DEBUG("doL1:" << m_doL1 << " L1pass:" << L1pass << " doL2SA:" << m_doL2SA << " L2SAPass:" << L2SApass << " doL2CB:" << m_doL2CB << " L2CBpass:" << L2CBpass <<
                " doEFSA:" << m_doEFSA << " EFSApass:" << EFSApass << " doEFCB:" << m_doEFCB <<  " EFCBpass:" << EFCBpass << " doEFIso:" << m_doEFIso << " EFIsopass:" << EFIsopass);

  //// Cuts based on the offline muon's features ////
  // Inclusive
  fill(m_group, muPt, L1pass, L2SApass, L2CBpass, EFSApass, EFCBpass, EFIsopass);

  // Plateau
  if(muPt>m_thresholds.at(chain)){
    fill(m_group, muEta, muPhi, averageMu, L1pass, L2SApass, L2CBpass, EFSApass, EFCBpass, EFIsopass);
  }

  return StatusCode::SUCCESS;
}



StatusCode TrigMuonEfficiencyMonMT :: selectMuonsTagAndProbe(SG::ReadHandle<xAOD::MuonContainer> &muons, std::vector<const xAOD::Muon*> &probes) const {

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
      double dimu_mass = (lvmu1+lvmu2).M()/1.e3;
      vec_invmass.push_back(dimu_mass);
      bool bit_mass = (dimu_mass > m_mass_lowlim) & (dimu_mass < m_mass_highlim);
      bool bit_dR = lvmu1.DeltaR(lvmu2)>0.5;
      if(m_use_extrapolator){
	const xAOD::TrackParticle *track1 = mu1->primaryTrackParticle();
	const Trk::TrackParameters *extTrack1 = m_matchTool->extTrackToPivot(track1);
	const xAOD::TrackParticle *track2 = mu2->primaryTrackParticle();
	const Trk::TrackParameters *extTrack2 = m_matchTool->extTrackToPivot(track2);
	if(extTrack1 && extTrack2){
	  TLorentzVector lvext1 = lvmu1;
	  TLorentzVector lvext2 = lvmu2;
	  double mupt1 = mu1->pt();
	  double mupt2 = mu2->pt();
	  lvext1.SetPtEtaPhiM( mupt1, extTrack1->position().eta(), extTrack1->position().phi(), mu1->m());
	  lvext2.SetPtEtaPhiM( mupt2, extTrack2->position().eta(), extTrack2->position().phi(), mu2->m());
	  bit_dR = lvext1.DeltaR(lvext2) > (MuonMatchingTool::reqdRL1byPt(mupt1)+MuonMatchingTool::reqdRL1byPt(mupt2));
	}
      }

      // For ttbar events
      if( m_eff_method == "TTbarTagAndProbe" && !bit_dR ) continue;
      // For Zmumu events
      if( m_eff_method == "ZTagAndProbe" && !(bit_mass && bit_dR) ) continue;
      
      dimuons.push_back(std::make_pair(mu1,mu2));
    }
  }// loop over muons

  auto invmass = Monitored::Collection(m_group+"_invmass", vec_invmass);
  fill(m_group, invmass);

  
  for (std::pair<const xAOD::Muon*,const xAOD::Muon*> dimu : dimuons){
    bool pass1 = false;
    bool pass2 = false;

    m_matchTool->matchEFIso(dimu.first, m_tag_trig, pass1);
    m_matchTool->matchEFIso(dimu.second, m_tag_trig, pass2);
    
    if(pass1){
      if(std::find(probes.begin(), probes.end(), dimu.second)==probes.end()){
	if(m_BarrelOnly){
          if( std::abs( dimu.second->eta() ) > 0. && std::abs( dimu.second->eta() ) < 1.05 ) probes.push_back(dimu.second);
        }
        else probes.push_back(dimu.second);
      }
    }
    if(pass2){
      if(std::find(probes.begin(), probes.end(), dimu.first)==probes.end()){
	if(m_BarrelOnly){
          if( std::abs( dimu.first->eta() ) > 0. && std::abs( dimu.first->eta() ) < 1.05 ) probes.push_back(dimu.first);
        }
        else probes.push_back(dimu.first);
      }
    }
  }
  
  return StatusCode::SUCCESS;;
}
