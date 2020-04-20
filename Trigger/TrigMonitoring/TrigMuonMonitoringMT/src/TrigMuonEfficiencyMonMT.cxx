/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  ATH_CHECK( m_matchTool.retrieve() );
  ATH_CHECK( m_MuonRoIContainerKey.initialize() );

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

  if(m_eff_method=="TagAndProbe"){
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


  SG::ReadHandle<xAOD::MuonRoIContainer> rois(m_MuonRoIContainerKey, ctx);
  if (! rois.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain xAOD::MuonRoI collection with name "<< m_MuonRoIContainerKey);
    return StatusCode::FAILURE;
  }
  if(rois->getConstStore()==nullptr){
    xAOD::MuonRoIContainer *ncptr = const_cast<xAOD::MuonRoIContainer*>(rois.get());
    ncptr->setStore(DataLink<SG::IConstAuxStore>(m_MuonRoIContainerKey.key()+"Aux.", ctx));
  }

  muPt = mu->pt()/1e3;
  muEta = mu->eta();
  muPhi = mu->phi();

  auto L1pass = Monitored::Scalar<bool>(m_group+"_L1pass",false);
  auto L2SApass = Monitored::Scalar<bool>(m_group+"_L2SApass",false);
  auto L2CBpass = Monitored::Scalar<bool>(m_group+"_L2CBpass",false);
  auto EFpass = Monitored::Scalar<bool>(m_group+"_EFpass",false);


  if(m_doL1){
    bool activestate = false;
    ATH_CHECK( m_matchTool->matchL1(mu, rois, m_l1seeds.at(chain), activestate) );
    L1pass = activestate;
  } else {
    L1pass = true;
  }


  if(L1pass){
    if(m_doL2SA){
      bool activestate = false;
      ATH_CHECK( m_matchTool->matchSA(mu, chain, activestate) );
      L2SApass = activestate;
    } else {
      L2SApass = true;
    }
  }


  if(L2SApass){
    if(m_doL2CB){
      bool activestate = false;
      ATH_CHECK( m_matchTool->matchCB(mu, chain, activestate) );
      L2CBpass = activestate;
    } else {
      L2CBpass = true;
    }
  }


  if(L2CBpass){
    if(m_doEF){
      bool activestate = false;
      ATH_CHECK( m_matchTool->matchEF(mu, chain, activestate) );
      EFpass = activestate;
    } else {
      EFpass = true;
    }
  }
  ATH_MSG_DEBUG("L1pass:" << L1pass << " L2SAPass:" << L2SApass << " L2CBpass:" << L2CBpass << " EFpass:" << EFpass);


  //// Cuts based on the offline muon's features ////
  // Inclusive
  fill(m_group, muPt, L1pass, L2SApass, L2CBpass, EFpass);

  // Plateau
  if(muPt>m_thresholds.at(chain)){
    fill(m_group, muEta, muPhi,  L1pass, L2SApass, L2CBpass, EFpass);
  }




  return StatusCode::SUCCESS;
}



StatusCode TrigMuonEfficiencyMonMT :: selectMuonsTagAndProbe(SG::ReadHandle<xAOD::MuonContainer> &muons, std::vector<const xAOD::Muon*> &probes) const {

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

      TLorentzVector lvmu1 = mu1->p4();
      TLorentzVector lvmu2 = mu2->p4();
      //double dimu_mass = (lvmu1+lvmu2).M()/1.e3;
      //bool bit_mass = (dimu_mass > m_mass_lowlim) & (dimu_mass < m_mass_highlim);
      //bool bit_OS = mu1->charge()*mu2->charge()>0;
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
      if(/*!bit_mass|!bit_OS|*/!bit_dR) continue;
      
      dimuons.push_back(std::make_pair(mu1,mu2));
    }
  }// loop over muons
  
  for (std::pair<const xAOD::Muon*,const xAOD::Muon*> dimu : dimuons){
    bool pass1 = false;
    bool pass2 = false;

    ATH_CHECK(m_matchTool->matchEF(dimu.first, m_tag_trig, pass1));
    ATH_CHECK(m_matchTool->matchEF(dimu.second, m_tag_trig, pass2));
    
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
