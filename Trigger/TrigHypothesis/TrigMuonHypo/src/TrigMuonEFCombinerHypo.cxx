/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonEFCombinerHypo.h"

#include "xAODMuon/MuonContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODEventInfo/EventInfo.h"

class ISvcLocator;

TrigMuonEFCombinerHypo::TrigMuonEFCombinerHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
  m_lumiTool("LuminosityTool/LuminosityTool")
{
        declareProperty("AcceptAll", m_acceptAll=true);
	declareProperty("RejectCBmuons", m_rejectCBmuons=false);
	std::vector<float> def_bins;
	def_bins.push_back(0);
	def_bins.push_back(9.9);
	std::vector<float> def_thrs;
	def_thrs.push_back(0.*CLHEP::GeV);
	declareProperty("PtBins", m_ptBins=def_bins);
	declareProperty("PtThresholds", m_ptThresholds=def_thrs);
	declareProperty("LumiTool", m_lumiTool);
	declareMonitoredStdContainer("Pt",  m_fex_pt);
	declareMonitoredStdContainer("Eta", m_fex_eta);
	declareMonitoredStdContainer("Phi", m_fex_phi);
	declareMonitoredVariable("BCID", m_fex_bcid);
	declareMonitoredVariable("Rate", m_fex_rate);

	m_bins = 0;
}

TrigMuonEFCombinerHypo::~TrigMuonEFCombinerHypo(){
}

HLT::ErrorCode TrigMuonEFCombinerHypo::hltInitialize(){

	if(m_acceptAll) {
		msg() << MSG::INFO
		<< "Accepting all the events with not cut!"
		<< endreq;
	} else {
		m_bins = m_ptBins.size() - 1;
		if (m_bins != m_ptThresholds.size()) {
			msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
			return HLT::BAD_JOB_SETUP;
		}
		for (std::vector<float>::size_type i=0; i<m_bins;++i) {
			msg() << MSG::INFO
			<< "bin " << m_ptBins[i] << " - " <<  m_ptBins[i+1]
			                                               << " with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV
			                                               << " CLHEP::GeV" << endreq;
		}
	}
	if(m_lumiTool.retrieve().isFailure()){
	  msg() <<MSG::WARNING<<"Unable to retrieve luminosity tool"<<endreq;
	}
	else{
	  msg()<<MSG::DEBUG<<"Sucessfully retrieved luminosty tool"<<endreq; 
	}
	msg() << MSG::INFO
	<< "Initialization completed successfully"
	<< endreq;

	return HLT::OK;
}


HLT::ErrorCode TrigMuonEFCombinerHypo::hltFinalize()
{
	msg() << MSG::INFO << "in finalize()" << endreq;
	return HLT::OK;
}


HLT::ErrorCode TrigMuonEFCombinerHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endreq;

	//resetting the monitoring variables
	m_fex_pt.clear();
	m_fex_eta.clear();
	m_fex_phi.clear();
	m_fex_rate=-1;
	m_fex_bcid=-1;

	if(m_acceptAll) {
		pass = true;
		if(msgLvl() <= MSG::DEBUG) {
			msg() << MSG::DEBUG
			<< "Accept property is set: taking all the events"
			<< endreq;
		}
		return HLT::OK;
	}

	bool result = false;
	bool debug = msgLvl() <= MSG::DEBUG;
	if(m_rejectCBmuons) result=true;
	// Some debug output:
	if(debug) msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;

	// Get the muon container from the outputTE
	const xAOD::MuonContainer* muonContainer(0);
	if(getFeature(outputTE, muonContainer)!=HLT::OK || muonContainer==0) {
	  if (debug) msg() << MSG::DEBUG << "no MuonContainer Feature found" << endreq;
	  return HLT::MISSING_FEATURE;
	}

	if(debug) ATH_MSG_DEBUG("Found MuonContainer, n(muons) = " << muonContainer->size());

	// loop on the muons within the RoI
	unsigned int j=0;
	for(auto muon : *muonContainer) {

	  msg() << MSG::DEBUG << "Looking at muon " << j++ << endreq;
	  const xAOD::Muon::MuonType muontype = muon->muonType();
	  if(muontype == xAOD::Muon::MuonType::Combined || muontype == xAOD::Muon::MuonType::SegmentTagged ) { // combined or segment tagged muon

	    // apparently match chi2 not stored yet?
	    //float matchchi2(-1);
	    //muon->parameter(matchchi2, xAOD::Muon::msInnerMatchChi2);
	    if (debug) msg() << MSG::DEBUG
			     << "Retrieved muon with pt "
			     << muon->pt()/CLHEP::GeV << " GeV." << endreq; //Match chi2 = " << matchchi2 << endreq;

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
	    if(debug) msg() << MSG::DEBUG << " REGTEST muon pt is " << muon->pt()/CLHEP::GeV << " CLHEP::GeV "
			//<< " with Charge " << muon->Charge()
			    << " and threshold cut is " << threshold/CLHEP::GeV << " CLHEP::GeV"
			    << " so hypothesis is " << (hypo_ok?"true":"false") << endreq;
	  }//combined or segment tagged muon
	  else{
	    if(m_rejectCBmuons){
	      result = true;
	    }
	  }
	}//loop on muons		     		
       
	pass = result;

	//Add some per bcid monitoring of rates to see if there are effects from RF detuning
	const xAOD::EventInfo* pEvent(0);
	if(store()->retrieve(pEvent).isFailure()){
	  msg() << MSG::ERROR << "Cannot find xAODEventInfo object" << endreq;
	  return HLT::ERROR;
	}

	double crosssection=1.0;
	//cross sections are ~ constant vs luminosity - taken from run 311481 (and are approximate
	// we only care how much the rates differ between bcids, so this should be enough).
	if(m_ptThresholds[0]/CLHEP::GeV>3.5 && m_ptThresholds[0]/CLHEP::GeV<4.0) crosssection=65.;
	if(m_ptThresholds[0]/CLHEP::GeV>5.5 && m_ptThresholds[0]/CLHEP::GeV<6.0) crosssection=55.;
	if(m_ptThresholds[0]/CLHEP::GeV>13.0 && m_ptThresholds[0]/CLHEP::GeV<14.0) crosssection=45.;
	if(m_ptThresholds[0]/CLHEP::GeV>25.0 && m_ptThresholds[0]/CLHEP::GeV<26.0) crosssection=30.;
	if(pass==true){
	  m_fex_bcid = pEvent->bcid();

	  if(m_lumiTool){
	    m_fex_rate = m_lumiTool->lbLuminosityPerBCID()*crosssection;
	  }
	}
	return HLT::OK;
}  
