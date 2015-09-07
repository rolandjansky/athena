/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

//#include "MooEvent/MooTrackContainer.h"
//#include "MuidEvent/MuidTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonHypo/TrigMooreHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

TrigMooreHypo::TrigMooreHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){        
        std::vector<float> def_bins;
	def_bins.push_back(0);
	def_bins.push_back(2.5); 
	std::vector<float> def_thrs;
	def_thrs.push_back(6.0*CLHEP::GeV);
	 
        declareProperty("PtBins", m_ptBins=def_bins);
	declareProperty("PtThresholds", m_ptThresholds=def_thrs);
	declareProperty("AcceptAll", m_acceptAll=false);
	
	declareMonitoredVariable("Pt",  m_fex_pt);
	declareMonitoredVariable("Eta", m_fex_eta);
	declareMonitoredVariable("Phi", m_fex_phi);

	m_bins = 0;
        m_storeGate = 0;
}

TrigMooreHypo::~TrigMooreHypo(){
}

HLT::ErrorCode TrigMooreHypo::hltInitialize(){

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
		<< " GeV" << endreq;
      }
  }
 
  msg() << MSG::INFO 
        << "Initialization completed successfully" 
        << endreq;
  
  return HLT::OK;
}


HLT::ErrorCode TrigMooreHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigMooreHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  m_storeGate = store();

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endreq;

  HLT::ErrorCode status;

  bool result = false;
  
  m_fex_pt = -1000;
  m_fex_eta = -1000;
  m_fex_phi = -1000;

   // Some debug output:
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;

  // Get vector of pointers to all TrigMuonEF objects linked to the outputTE 
  //   by label "uses":
  std::vector<const TrigMuonEFContainer*> vectorOfTrigMuonEF;
  status = getFeatures(outputTE, vectorOfTrigMuonEF);
  if(status!=HLT::OK) {
      msg() << MSG::DEBUG << "no TrigMoore Feature found" << endreq;
      return status;
  } else msg() << MSG::DEBUG << "TrigMoore Feature found" << endreq;


  // Check that there is only one MuonFeature
  if (vectorOfTrigMuonEF.size() != 1){
    msg() << MSG::DEBUG
        << "Size of vector is not 1 but "<<vectorOfTrigMuonEF.size() 
        << endreq;
    if (vectorOfTrigMuonEF.size() == 0) return HLT::NAV_ERROR;
  }  else msg() << MSG::DEBUG << "One TrigMoore container found" << endreq;

  
  float threshold = 0;
  for (unsigned int i=0; i<vectorOfTrigMuonEF.size(); i++){
    msg() << MSG::DEBUG << "Element "<<i<<" of vector of TrigMuonEF containers  "<<endreq;
    // Get first (and only) RoI:
    const TrigMuonEFContainer * trackCont = vectorOfTrigMuonEF[i];
    if(!trackCont){
      msg() << MSG::ERROR
	    << "Retrieval of TrigMuonEF  container from vector failed"
	    << endreq;
      return  HLT::NAV_ERROR;
    }  else msg() << MSG::DEBUG << "container OK with size "<<trackCont->size()<< endreq;
    
    
    // Check pt threshold for hypothesis,
    for (TrigMuonEFContainer::const_iterator tr = trackCont->begin();
	 tr != trackCont->end(); tr++){
      msg() << MSG::DEBUG << " Muon code is <" << (*tr)->MuonCode() <<">"<< endreq;
      if ((*tr)->MuonCode()>100)
	{
	  msg() << MSG::DEBUG <<" track code is "<<(*tr)->MuonCode() << endreq;
	  msg() << MSG::DEBUG <<"abs pt of this track is" << (*tr)->pt()/CLHEP::GeV << " GeV" << endreq;
	  
	  m_fex_pt  = (*tr)->pt()/CLHEP::GeV;
	  float theta = atan( 1./(*tr)->cotTh() );
          m_fex_eta = -log(fabs(tan(theta/2.)));
          m_fex_phi = (*tr)->phi();  
	  

          float absEta = fabs(m_fex_eta);
          for (std::vector<float>::size_type i=0; i<m_bins; ++i)
	    if ( absEta > m_ptBins[i] && absEta < m_ptBins[i+1] ) threshold = m_ptThresholds[i]; 
	  
	  if ((*tr)->pt()/CLHEP::GeV > (threshold/CLHEP::GeV)){
	    result = true;
	  }
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " REGTEST muon pt is " << (*tr)->pt()/CLHEP::GeV << " GeV "   
					   << " with Charge " << (*tr)->Charge() 
					   << " and threshold cut is " << threshold/CLHEP::GeV << " GeV" 
					   << " so hypothesis is " << (result?"true":"false") << endreq;
	  
	}
    }
  }
  
  pass = result;
  return HLT::OK;
  
}  
