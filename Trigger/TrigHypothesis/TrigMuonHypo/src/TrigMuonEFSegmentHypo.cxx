/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonEFSegmentHypo.h"

#include "xAODMuon/MuonContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

TrigMuonEFSegmentHypo::TrigMuonEFSegmentHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){
  declareProperty("AcceptAll", m_acceptAll=true);
  declareProperty("RejectCBmuons", m_rejectCBmuons=false);
  std::vector<float> def_bins;
  def_bins.push_back(0);
  def_bins.push_back(9.9);
  std::vector<float> def_thrs;
  def_thrs.push_back(0.*CLHEP::GeV);
  declareProperty("PtBins", m_ptBins=def_bins);
  declareProperty("PtThresholds", m_ptThresholds=def_thrs);
  declareMonitoredStdContainer("Pt",  m_fex_pt);
  declareMonitoredStdContainer("Eta", m_fex_eta);
  declareMonitoredStdContainer("Phi", m_fex_phi);

  m_bins = 0;
}

TrigMuonEFSegmentHypo::~TrigMuonEFSegmentHypo(){
}

HLT::ErrorCode TrigMuonEFSegmentHypo::hltInitialize(){

  if(m_acceptAll) {
    ATH_MSG_INFO("Accepting all the events with not cut!");
  } else {
    m_bins = m_ptBins.size() - 1;
    if (m_bins != m_ptThresholds.size()) {
      ATH_MSG_INFO("bad thresholds setup .... exiting!");
      return HLT::BAD_JOB_SETUP;
    }
    for (std::vector<float>::size_type i=0; i<m_bins;++i) {
      ATH_MSG_INFO("bin " << m_ptBins[i] << " - " <<  m_ptBins[i+1]
	    << " with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV
		   << " CLHEP::GeV");
    }
  }

  ATH_MSG_INFO("Initialization completed successfully");

  return HLT::OK;
}


HLT::ErrorCode TrigMuonEFSegmentHypo::hltFinalize()
{
  ATH_MSG_INFO("in finalize()");
  return HLT::OK;
}


HLT::ErrorCode TrigMuonEFSegmentHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  if(msgLvl() <= MSG::DEBUG) ATH_MSG_DEBUG("in execute()");

  //resetting the monitoring variables
  m_fex_pt.clear();
  m_fex_eta.clear();
  m_fex_phi.clear();

  if(m_acceptAll) {
    pass = true;
    if(msgLvl() <= MSG::DEBUG) {
      ATH_MSG_DEBUG("Accept property is set: taking all the events");
    }
    return HLT::OK;
  }

  bool result = false;
  bool debug = msgLvl() <= MSG::DEBUG;
  if(m_rejectCBmuons) result=true;
  // Some debug output:
  if(debug) ATH_MSG_DEBUG("outputTE->ID(): " << outputTE->getId());

  // Get the muon container from the outputTE
  const xAOD::MuonContainer* muonContainer(0);
  if(getFeature(outputTE, muonContainer)!=HLT::OK || muonContainer==0) {
    if (debug) ATH_MSG_DEBUG("no MuonContainer Feature found");
    return HLT::MISSING_FEATURE;
  }

  if(debug) ATH_MSG_DEBUG("Found MuonContainer, n(muons) = " << muonContainer->size());

  // loop on the muons within the RoI
  for(auto muon : *muonContainer) {
    const xAOD::Muon::MuonType muontype = muon->muonType();
    if(muontype == xAOD::Muon::MuonType::SegmentTagged ) {
      // apparently match chi2 not stored yet?
      //float matchchi2(-1);
      //muon->parameter(matchchi2, xAOD::Muon::msInnerMatchChi2);
      if (debug) ATH_MSG_DEBUG("Retrieved muon with pt " << muon->pt()/CLHEP::GeV << " GeV."); //Match chi2 = " << matchchi2 << endreq;

      m_fex_pt.push_back(muon->pt()/CLHEP::GeV);
      const float eta = muon->eta();
      m_fex_eta.push_back(eta);
      m_fex_phi.push_back(muon->phi());

      const float absEta = fabs(eta);
      float threshold = 0;
      for (std::vector<float>::size_type k=0; k<m_bins; ++k) {
	if (absEta > m_ptBins[k] && absEta <= m_ptBins[k+1]) threshold = m_ptThresholds[k];
      }
      bool hypo_ok = false;
      if (fabs(muon->pt())/CLHEP::GeV > (threshold/CLHEP::GeV)){
	result = true;
	hypo_ok = true;
	if(m_rejectCBmuons && muontype == xAOD::Muon::MuonType::Combined){
	  result = false;
	  hypo_ok = false;
	}
	else{
	  result = true;
	  hypo_ok=true;
	}
      }
      if(debug) ATH_MSG_DEBUG(" REGTEST muon pt is " << muon->pt()/CLHEP::GeV << " CLHEP::GeV "
			      << " and threshold cut is " << threshold/CLHEP::GeV << " CLHEP::GeV"
			      << " so hypothesis is " << (hypo_ok?"true":"false"));
    }
    else{
      if(m_rejectCBmuons){
	result = true;
      }
    }
  }//loop on muons		     		
       
  pass = result;
  return HLT::OK;
}  
