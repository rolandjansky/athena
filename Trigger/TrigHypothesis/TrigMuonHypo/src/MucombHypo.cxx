/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonHypo/MucombHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

MucombHypo::MucombHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

    std::vector<float> def_bins;
    def_bins.push_back(0);
    def_bins.push_back(2.5); 
    std::vector<float> def_thrs;
    def_thrs.push_back(5.83*CLHEP::GeV);
	 
    declareProperty("PtBins", m_ptBins=def_bins);
    declareProperty("PtThresholds", m_ptThresholds=def_thrs);

    declareProperty("AcceptAll", m_acceptAll=false);

    // Enable pi/K decay rejection
    declareProperty("Apply_pik_Cuts",        m_pik_cuts=false);
    declareProperty("MaxPtToApply_pik",      m_maxPtToApply_pik=25.);
    declareProperty("MaxChi2ID_pik",         m_chi2MaxID=3.5);

    // strategy dependent pT cuts (B/T/S = 0 fields, L1 seeded, ...)
    //1: Std: muFast-backextrapolated-ID match
    //2: Toroid OFF - Solenoid ON  muFast-ID match   
    //3: Toroid ON  - Solenoid OFF muFast-ID match   
    //4: Toroid OFF - Solenoid OFF muFast-ID match 
    //5: Toroid ON  - Solenoid ON  L1-ID match
    //6: Toroid OFF - Solenoid ON  L1-ID match
    //7: Toroid ON  - Solenoid OFF L1-ID match
    //8: Toroid OFF - Solenoid OFF L1-ID match
    declareProperty("ApplyStrategyDependentCuts", m_strategydependent = false);
    declareProperty("StrategyDependentPtCuts",    m_strategyDependentPtCuts);

    std::vector<double> def_strategyDependentPtCuts;
    def_strategyDependentPtCuts.push_back(  0.0 );  //strategy 1 (in GeV) (if<0 do not apply in addition the standrad signature cut)
    def_strategyDependentPtCuts.push_back( -3.0 );  //strategy 2 (in GeV)
    def_strategyDependentPtCuts.push_back( -3.0 );  //strategy 3 (in GeV)
    def_strategyDependentPtCuts.push_back( -3.0 );  //strategy 4 (in GeV)
    def_strategyDependentPtCuts.push_back(  0.0 );  //strategy 5 (in GeV)
    def_strategyDependentPtCuts.push_back( -3.0 );  //strategy 6 (in GeV)
    def_strategyDependentPtCuts.push_back( -3.0 );  //strategy 7 (in GeV)
    def_strategyDependentPtCuts.push_back( -3.0 );  //strategy 8 (in GeV)
    m_strategyDependentPtCuts.set( def_strategyDependentPtCuts );

    declareMonitoredVariable("Pt",            m_fex_pt);
    declareMonitoredVariable("PtFL",          m_ptFL);
    declareMonitoredVariable("StrategyFlag",  m_Strategy);
    declareMonitoredVariable("Eta",           m_id_eta);
    declareMonitoredVariable("Phi",           m_id_phi);
    declareMonitoredVariable("Z0",            m_id_Z0);
    declareMonitoredVariable("A0",            m_id_A0);

    m_bins = 0;
}

MucombHypo::~MucombHypo(){
}

HLT::ErrorCode MucombHypo::hltInitialize(){
    
  msg() << MSG::INFO << "Initializing " << name() << " - package version " 
	<< PACKAGE_VERSION << endreq;

  if(m_acceptAll) {
      msg() << MSG::INFO
            << "Accepting all the events with not cut!"
	    << endreq;
  } else {      
      m_bins = m_ptBins.size() - 1;
      if (m_bins != m_ptThresholds.size()) {
          msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
          return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
      
      for (std::vector<float>::size_type i=0; i<m_bins;++i) {
      
          msg() << MSG::INFO
                << "bin " << m_ptBins[i] << " - " <<  m_ptBins[i+1]
		<< " with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV 
		<< " GeV" << endreq;
      }

  }
 
  msg() << MSG::INFO 
        << "Initialization completed successfully" 
        << endreq;
  
  return HLT::OK;
}


HLT::ErrorCode MucombHypo::hltFinalize(){
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}


HLT::ErrorCode  MucombHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){
  
  m_storeGate = store();
  
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endreq;
  
  if(m_acceptAll) {
    pass = true;
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endreq;
    }
    return HLT::OK;
  }
  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Accept property not set: applying selection!" << endreq;
  }
  
  bool result = false;
  
  // Some debug output: 
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;
  }
  
  
  ElementLink<CombinedMuonFeatureContainer> CombinedMuonFeatureEL;
  if ( HLT::OK != getFeatureLink<CombinedMuonFeatureContainer, 
       CombinedMuonFeature>(outputTE, CombinedMuonFeatureEL) ) {                                                                     
    msg() << MSG::ERROR << " getFeatureLink fails to get the CombinedMuonFeature " << endreq;                                      
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }                                                           
  
  if ( !CombinedMuonFeatureEL.isValid() ) { 
    msg() << MSG::ERROR << " getFeatureLink finds no CombinedMuonFeature (EL invalid)" << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
  } 
  
  // Get first (and only) RoI:
  const CombinedMuonFeature* pMuon = *CombinedMuonFeatureEL; 
  if(!pMuon){
    msg() << MSG::ERROR << "Retrieval of CombinedMuonFeature from vector failed" << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
  }

  m_fex_pt    = (pMuon && pMuon->ptq() )? pMuon->ptq()/CLHEP::GeV : -9999;
  m_ptFL      = m_fex_pt;
  m_id_eta    = (pMuon && pMuon->ptq() )? pMuon->eta() : -9999;
  m_id_phi    = (pMuon && pMuon->ptq() )? pMuon->phi() : -9999;
  int usealgo      = (pMuon)? pMuon->comb_strategy() : -1;
  float ptres_comb = (pMuon)? pMuon->sigma_pt() : -1;

  m_Strategy  = usealgo;

  //get the pointer to the muon feature
  ElementLink<MuonFeatureContainer> muonFeatureEL = pMuon->muFastTrackLink();           
  const MuonFeature* pMuonFeature = 0;
  if ( !muonFeatureEL.isValid() ) { 
    msg() << MSG::DEBUG << " CombinedMuonFeature has no valid MuonFeature (EL invalid)" << endreq;
    if (usealgo < 5) {//reject feature if muFast required
      pass = false;
      return HLT::OK;
    }
  } 
  else {
    pMuonFeature = *muonFeatureEL;
  }

  //get the pointer to the ID track
  ElementLink<TrigInDetTrackCollection> trackEL = pMuon->IDTrackLink();           
  const TrigInDetTrack* track = 0;
  if ( !trackEL.isValid() ) {//ID track always required, if absent reject feature
    msg() << MSG::DEBUG << " CombinedMuonFeature has no valid IDtracks (EL invalid)" << endreq;
    pass = false;
    return HLT::OK;
  } 
  else {
    track = *trackEL;
  }

  m_id_A0     = (track)? track->param()->a0() : -9999;
  m_id_Z0     = (track)? track->param()->z0()/10. : -9999;
  
  // fill Monitoring histos

  if(msgLvl() <= MSG::DEBUG) {
     msg() << MSG::DEBUG << " SigmaPt in HYPO is: " << ptres_comb 
           << " / combined muon reconstructed with strategy: " << usealgo
           << endreq;
  }

  //Get the Pt cut for that eta bin
  float threshold = (m_id_eta!=-9999)? 0 : 99999999;
  float absEta = fabs(m_id_eta);
  for (std::vector<float>::size_type i=0; i<m_bins; ++i)
    if ( absEta > m_ptBins[i] && absEta < m_ptBins[i+1] ) threshold = m_ptThresholds[i]; 

  // Check pt threshold for hypothesis and pi/k rejection cuts, 
  // convert units since Muonfeature is in GeV

  //Kpi rejection
  bool pik_cut = true; 
  if(m_pik_cuts && track && pMuonFeature && fabsf(pMuon->ptq()/1000.) < m_maxPtToApply_pik ) {
    if (pMuon->IDTrack()->chi2() > m_chi2MaxID ) pik_cut = false; 
  }
  
  //Std Pt cut
  bool std_cut = true; 
  if ( fabsf(pMuon->ptq()/1000.) <= (threshold/CLHEP::GeV) ) std_cut = false;
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " REGTEST muon pt is " << pMuon->ptq()/1000. 
	  << " GeV and threshold cut is " << threshold/CLHEP::GeV
	  << " GeV and pik_cut is " << (pik_cut? "true":"false") << endreq;
  }

  //Strategy dependent Pt cuts
  bool sdp_cut = true; 
  if (m_strategydependent && usealgo > 0) {
    if (usealgo >= 1 && usealgo <= 8) {
      double tmpcut = m_strategyDependentPtCuts.value()[usealgo-1];
      if (tmpcut > 0) {
	if ( fabsf(pMuon->ptq()/1000.) <= tmpcut ) sdp_cut = false;
      }	
      else {//Do not apply std Pt cut
	std_cut = true; 
	if ( fabsf(pMuon->ptq()/1000.) <= fabs(tmpcut) ) sdp_cut = false;
      }
      if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << " REGTEST muon pt is " << pMuon->ptq()/1000. 
	      << " and threshold for strategy dependent cut is " << tmpcut 
	      << " GeV and strategy dependent cuts is " << (sdp_cut? "true":"false") << endreq;
      }
    }
    else {
      if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "usealgo out of range, is: " << usealgo << " while should be in [1, 8]" << endreq;
      }
    }
  }

  result = std_cut && pik_cut && sdp_cut;

  if (result) m_ptFL = -9999.;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " REGTEST muon passed pt threshold: " <<  (std_cut? "true":"false")
	  << " and pik_cut is " << (pik_cut? "true":"false") 
	  << " and strategy dependent cuts is " << (sdp_cut? "true":"false") 
	  << " so hypothesis is " << (result?"true":"false") << endreq;
  }

  //store the result
  pass = result;
  
  return HLT::OK;
}
