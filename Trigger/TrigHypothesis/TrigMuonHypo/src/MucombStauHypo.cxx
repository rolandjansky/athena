/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonHypo/MucombStauHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

MucombStauHypo::MucombStauHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
  m_backExtrapolator("TrigMuonBackExtrapolator")
{
    declareProperty( "BackExtrapolator", m_backExtrapolator, "public tool for back extrapolating the muon tracks to the IV");
    std::vector<float> def_bins;
    def_bins.push_back(0);
    def_bins.push_back(2.5); 
    std::vector<float> def_thrs;
    def_thrs.push_back(50*CLHEP::GeV);
	 
    declareProperty("PtBins", m_ptBins=def_bins);
    declareProperty("PtThresholds", m_ptThresholds=def_thrs);
    declareProperty("BetaMax", m_betaMax = 0.97);
    declareProperty("MMin", m_mMin = 100.*CLHEP::GeV);
    declareProperty("Apply_pik_Cuts", m_pik_cuts=false);
    declareProperty("AcceptAll", m_acceptAll=false);
	
    declareMonitoredVariable("Pt", m_fex_pt);
    declareMonitoredVariable("Eta", m_id_eta);
    declareMonitoredVariable("Phi", m_id_phi);
    declareMonitoredVariable("Z0", m_id_Z0);
    declareMonitoredVariable("A0", m_id_A0);
    declareMonitoredVariable("Mass", m_fex_mass);
    declareMonitoredVariable("Beta", m_fex_beta);
    declareMonitoredVariable("IDMass", m_id_mass);

    m_storeGate = 0;
    m_bins = 0;
}

MucombStauHypo::~MucombStauHypo(){
}

HLT::ErrorCode MucombStauHypo::hltInitialize(){
    
  msg() << MSG::INFO << "Initializing " << name() << " - package version " 
	<< PACKAGE_VERSION << endmsg;

  // ===================== GET THE BACK EXTRAPOLATOR =========================
  StatusCode sc = m_backExtrapolator.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve "<<m_backExtrapolator<< endmsg;
    return HLT::BAD_JOB_SETUP;
  } else
  {
    msg() << MSG::INFO << "Retrieved tool " << m_backExtrapolator << endmsg;
  }
  
  if(m_acceptAll) {
      msg() << MSG::INFO
            << "Accepting all the events with not cut!"
	    << endmsg;
  } else {      
      m_bins = m_ptBins.size() - 1;
      if (m_bins != m_ptThresholds.size()) {
          msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endmsg;
          return HLT::BAD_JOB_SETUP;
      }
      
      for (std::vector<float>::size_type i=0; i<m_bins;++i) {
      
          msg() << MSG::INFO
                << "bin " << m_ptBins[i] << " - " <<  m_ptBins[i+1]
		<< " with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV 
		<< " CLHEP::GeV" << endmsg;
      }

  }
  msg() << MSG::INFO 
        << "Initialization completed successfully" 
        << endmsg;
  
  return HLT::OK;
}


HLT::ErrorCode MucombStauHypo::hltFinalize(){
  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}


HLT::ErrorCode  MucombStauHypo::hltExecute(const HLT::TriggerElement* outputTE, 
                                      bool& pass){
  
  m_storeGate = store();


  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endmsg;
  
  if(m_acceptAll) {
      pass = true;
      if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG 
	        << "Accept property is set: taking all the events"
		<< endmsg;
      }
      return HLT::OK;
  }
  
  if(msgLvl() <= MSG::DEBUG) {
       msg() << MSG::DEBUG << "Accept property not set: applying selection!"
             << endmsg;
  }


  bool result = false;

  
  // Some debug output: 
  if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId()
            << endmsg;
  }
  
  
  ElementLink<CombinedMuonFeatureContainer> CombinedMuonFeatureEL;
  if ( HLT::OK != getFeatureLink<CombinedMuonFeatureContainer, 
                    CombinedMuonFeature>(outputTE, CombinedMuonFeatureEL) )            
  {                                                                     
    msg() << MSG::ERROR 
          << " getFeatureLink fails to get the CombinedMuonFeature " << endmsg;                                      
  //  return HLT::ERROR;                                                  
  }                                                           
  
  /*
  std::vector<const CombinedMuonFeature*> vectorOfCombinedMuons; 
  HLT::ErrorCode status = getFeatures(outputTE,vectorOfCombinedMuons);
  if( status!=HLT::OK ) {
      msg() << MSG::DEBUG << "no CombinedMuonFeatures found" << endmsg;
      return status;
  }
  // Check that there is only one MuonFeature
  if (vectorOfCombinedMuons.size() != 1){
    msg() << MSG::ERROR << "Size of vector is " << vectorOfCombinedMuons.size()
          << endmsg;
    return HLT::NAV_ERROR;
  }
  */
  if ( !CombinedMuonFeatureEL.isValid() )     
  { 
    msg() << MSG::ERROR
          << " getFeatureLink finds no CombinedMuonFeature (EL invalid)"
	  << endmsg;
    return HLT::NAV_ERROR;
  } 
  
  // Get first (and only) RoI:
  const CombinedMuonFeature* pMuon = *CombinedMuonFeatureEL; 
  if(!pMuon){
    msg() << MSG::DEBUG << "Retrieval of CombinedMuonFeature from vector failed"
          << endmsg;
    return HLT::NAV_ERROR;
  }


  //get the pointer to the muon feature
  ElementLink<MuonFeatureContainer> muonFeatureEL = pMuon->muFastTrackLink();           
  const MuonFeature* pMuonFeature;
  if ( !muonFeatureEL.isValid() )
  { 
    msg() << MSG::DEBUG 
          << " CombinedMuonFeature has no valid MuonFeature (EL invalid)" 
	  << endmsg;
    const MuonFeature* none = 0;
    pMuonFeature = none;
  } else {
    pMuonFeature = *muonFeatureEL;
  }

  //
  bool hasTrack = false;
  //get the pointer to the ID track
  ElementLink<TrigInDetTrackCollection> trackEL = pMuon->IDTrackLink();           
  const TrigInDetTrack* track;
  if ( !trackEL.isValid() )
  { 
    msg() << MSG::DEBUG 
          << " CombinedMuonFeature has no valid IDtracks (EL invalid)" 
	  << endmsg;
    track = 0;
    hasTrack= false;
  } else {
    track = *trackEL;
    hasTrack= true;
  }
  
 double mCand = -999.;
  m_fex_mass = -999.;
  m_fex_beta = 0.;
  m_id_mass = -999.;

  if (hasTrack)
    {
      // fill Monitoring histos
      //const  TrigInDetTrack* track = pMuon->IDTrack();
      m_fex_pt = (pMuon && pMuon->ptq() )? pMuon->ptq()/CLHEP::GeV : -9999;
      m_id_eta = (pMuon && pMuon->ptq() )? pMuon->eta() : -9999;
      m_id_phi = (pMuon && pMuon->ptq() )? pMuon->phi() : -9999;
      m_id_A0  = (track)? track->param()->a0() : -9999;
      m_id_Z0  = (track)? track->param()->z0()/10. : -9999;
      
      //Get the Pt cut for that eta bin
      float threshold = (m_id_eta!=-9999)? 0 : 999999;
      float absEta = fabs(m_id_eta);
      for (std::vector<float>::size_type i=0; i<m_bins; ++i)
      if ( absEta > m_ptBins[i] && absEta < m_ptBins[i+1] ) threshold = m_ptThresholds[i]; 
      // Check pt threshold for hypothesis and back extrapolator cuts, 
      // convert units since Muonfeature is in GeV
      /*
      bool pik_cut = true; 
      if(m_pik_cuts && track && pMuonFeature && fabsf(pMuon->ptq()/1000.) < 25. )
      {
      double win_pt = 6.;
      if (name().find("4GeV")!=std::string::npos) win_pt = 4.;
      if ( (m_backExtrapolator->tight_window_match(pMuon->ptq()/1000.,
      pMuon->muFastTrack(), pMuon->IDTrack(), win_pt, 0.9)).isFailure() )
      {
      pik_cut &= false; 
      }
      if (pMuon->IDTrack()->chi2() > 3.5 ) pik_cut &= false; 
      }
      */
      if(pMuonFeature != NULL){
	if (fabsf(pMuonFeature->eta())<1.05)
	  {
	        
	    double theta = 2.*atan(exp(-pMuon->eta())); 
	    double pCand = std::abs(pMuon->ptq())/sin(theta);
	    double BetaCand = pMuonFeature->beta();
	    mCand = pCand * sqrt(1.-BetaCand*BetaCand)/BetaCand; 
	    m_id_mass = mCand; 
            m_fex_beta = BetaCand;
	    if (mCand >m_mMin && BetaCand<m_betaMax &&  std::abs(pMuon->ptq()/1000.) > (threshold/CLHEP::GeV))
	      result = true;
	  } 
	else
	  if ( std::abs(pMuon->ptq()/1000.) > (threshold/CLHEP::GeV) ){
	    result = true;
	  }
      }
    }
  
  
  else
    {
      if(pMuonFeature != NULL){
        float threshold = 0;
        float absEta = fabs(-pMuonFeature->eta());
        for (std::vector<float>::size_type i=0; i<m_bins; ++i)
        if ( absEta > m_ptBins[i] && absEta < m_ptBins[i+1] ) threshold = m_ptThresholds[i]; 
	if (fabsf(pMuonFeature->eta())<1.)
	  {
	    double theta = 2.*atan(exp(-pMuonFeature->eta())); 
	    double pCand = fabsf(pMuonFeature->pt())/sin(theta)*CLHEP::GeV;
	    double BetaCand = pMuonFeature->beta();
	    mCand = pCand * sqrt(1.-BetaCand*BetaCand)/BetaCand; 
	    if (mCand >m_mMin && BetaCand<m_betaMax && fabs(pMuonFeature->pt())>threshold/CLHEP::GeV) 
	      result = true;
	    m_fex_mass = mCand;   
            m_fex_beta = BetaCand;
	  }
	else
        {
	    if (fabs(pMuonFeature->pt())>threshold/CLHEP::GeV) 
	        result = true;
        } 
      }
    }
  
  //if (msgLvl() <= MSG::DEBUG) {
  msg() << MSG::DEBUG << " REGTEST muon pt is " << pMuon->ptq()/1000. 
	<< " CLHEP::GeV and threshold cut is "<<m_ptThresholds[0]<<" CLHEP::GeV" 
	<< " so hypothesis is " << (result?"true":"false") << endmsg;
  //}
  //store the result
  pass = result;
  
  return HLT::OK;
}
