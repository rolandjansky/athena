/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuGirlHypo.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

TrigMuGirlHypo::TrigMuGirlHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){
    declareProperty("AcceptAll", m_acceptAll=true);
    std::vector<float> def_bins;
    def_bins.push_back(0);
    def_bins.push_back(2.5); 
    std::vector<float> def_thrs;
    def_thrs.push_back(0.*CLHEP::GeV);
    declareProperty("PtBins", m_ptBins=def_bins);
    declareProperty("PtThresholds", m_ptThresholds=def_thrs);
    //    declareMonitoredStdContainer("Pt",  m_fex_pt);
    //    declareMonitoredStdContainer("Eta", m_fex_eta);
    //    declareMonitoredStdContainer("Phi", m_fex_phi);

    m_bins = 0;
    m_storeGate =0;
}

TrigMuGirlHypo::~TrigMuGirlHypo(){
}

HLT::ErrorCode TrigMuGirlHypo::hltInitialize(){

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


HLT::ErrorCode TrigMuGirlHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}


HLT::ErrorCode TrigMuGirlHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  m_storeGate = store();
  
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endreq;
  
  //resetting the monitoring variables 
  //  m_fex_pt.clear();
  //  m_fex_eta.clear();  
  //  m_fex_phi.clear();

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

  // Some debug output:
  //  if(debug) msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;

  const TrigMuonEFInfoContainer* trackCont = NULL;
  std::vector<const TrigMuonEFInfoContainer*> vectorOfTrigMuonEF;  
  HLT::ErrorCode status; 

  status=getFeatures(outputTE, vectorOfTrigMuonEF);

  if (status != HLT::OK || vectorOfTrigMuonEF.size()==0) {
      msg() << MSG::DEBUG << "Could not retieve vector of TrigMuonEFInfoConatiner "<< endreq;
      return HLT::MISSING_FEATURE;

  }

  float threshold = 0;
  
  for (unsigned int i=0; i<vectorOfTrigMuonEF.size(); i++)
    {
      msg() << MSG::DEBUG << "Element "<<i<<" of vector of TrigMuonEFInfo containers  "<<endreq;
      // Get first (and only) RoI:
      trackCont = vectorOfTrigMuonEF[i];
      if(!trackCont)
	{
	  msg() << MSG::ERROR
		<< "Retrieval of TrigMuonEFInfo container from vector failed" << endreq;
	  return  HLT::NAV_ERROR;
	}  
      else msg() << MSG::DEBUG << "container OK with size "<<trackCont->size()<< endreq;
    }
  if(trackCont != NULL){
    for (TrigMuonEFInfoContainer::const_iterator tr = trackCont->begin();tr != trackCont->end(); tr++)
      {
	const TrigMuonEFInfo* eInfo = (*tr);
	const TrigMuonEFInfoTrackContainer* trI=eInfo->TrackContainer();
	msg() << MSG::DEBUG << "Track container size: " <<  trI->size() << endreq;
	for (TrigMuonEFInfoTrackContainer::const_iterator  Ir= trI->begin();Ir != trI->end(); Ir++)
	  {
	    const TrigMuonEFInfoTrack* muonInfo = (*Ir);
	    TrigMuonEFCbTrack* trC = muonInfo->CombinedTrack(); //also TrigMuGirl track      
	    
	    
	    double eta=-log(sqrt(1+((trC)->cotTh())*((trC)->cotTh()))-((trC)->cotTh()));
	    float absEta = fabs(eta);
	    for (std::vector<float>::size_type k=0; k<m_bins; ++k) {
	      if (absEta > m_ptBins[k] && absEta <= m_ptBins[k+1]) threshold = m_ptThresholds[k]; 
	    }
	    if (fabs((trC)->pt())/CLHEP::GeV > (threshold/CLHEP::GeV)) 
	      {
		result = true;
	      }
	    if(debug) msg() << MSG::DEBUG << " REGTEST muon pt is " << (trC)->pt()/CLHEP::GeV << " GeV "   
			    << " with Charge " << (trC)->charge() 
			    << " and threshold cut is " << threshold/CLHEP::GeV << " GeV" 
			    << " so hypothesis is " << (result?"true":"false") << endreq;
	  }
      }
  }
  pass = result;
  return HLT::OK;
}  
