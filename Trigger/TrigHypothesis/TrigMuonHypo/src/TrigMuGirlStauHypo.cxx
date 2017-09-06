/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuGirlStauHypo.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

TrigMuGirlStauHypo::TrigMuGirlStauHypo(const std::string & name, ISvcLocator* pSvcLocator):
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

TrigMuGirlStauHypo::~TrigMuGirlStauHypo(){
}

HLT::ErrorCode TrigMuGirlStauHypo::hltInitialize(){

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


HLT::ErrorCode TrigMuGirlStauHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}


HLT::ErrorCode TrigMuGirlStauHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  m_storeGate = store();
  
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endmsg;
  
  //resetting the monitoring variables 
  //  m_fex_pt.clear();
  //  m_fex_eta.clear();  
  //  m_fex_phi.clear();

  if(m_acceptAll) {
      pass = true;
      if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG 
                << "Accept property is set: taking all the events"
                << endmsg;
      }
      return HLT::OK;
  }
  
  bool result = false;



  const TrigMuonEFInfoContainer* trackCont = NULL;
  std::vector<const TrigMuonEFInfoContainer*> vectorOfTrigMuonEF;  
  HLT::ErrorCode status;
 
  status=getFeatures(outputTE, vectorOfTrigMuonEF);

  if (status != HLT::OK || vectorOfTrigMuonEF.size()==0) {
      msg() << MSG::DEBUG << "Could not retieve vector of TrigMuonEFConatiner "<< endmsg;
      return HLT::MISSING_FEATURE;

  }

  
  for (unsigned int i=0; i<vectorOfTrigMuonEF.size(); i++)
    {
      msg() << MSG::DEBUG << "Element "<<i<<" of vector of TrigMuonEF containers  "<<endmsg;
      // Get first (and only) RoI:
      trackCont = vectorOfTrigMuonEF[i];
      if(!trackCont)
	{
	  msg() << MSG::ERROR
		<< "Retrieval of TrigMuonEF  container from vector failed" << endmsg;
	  return  HLT::NAV_ERROR;
	}  
      else msg() << MSG::DEBUG << "container OK with size "<<trackCont->size()<< endmsg;
    }
  if(trackCont!=NULL){
    for (const TrigMuonEFInfo* tr : *trackCont)
      {
	//if ((*tr)->MuonCode()<100 && (*tr)->MuonCode() != 5) continue;
	if(!tr->hasCombinedTrack()) continue;
	//      {
	
	//	m_pt.push_back( (*tr)->pt());
	//	m_cotTh.push_back( (*tr)->cotTh());
	//	m_phi.push_back( (*tr)->phi());
	//	m_m.push_back( (*tr)->m());
	//	m_charge.push_back( (*tr)->charge());
	
	//      }
	
	// loop on the muons within the RoI
	//TrigMuonEFInfoTrackContainer* InfoTrkCont= (*tr)->TrackContainer();
	//if ((*InfoTrkCont->begin())->MuonType() != 2) continue;
	const TrigMuonEFCbTrack* trk = nullptr;
        if (tr->hasLegacyTrack())
          trk = tr->legacyCombinedTrack();
        else
          trk = tr->TrackContainer()->front()->CombinedTrack();
	
	double eta=-log(sqrt(1+((trk)->cotTh())*((trk)->cotTh()))-((trk)->cotTh()));
	double pt = (trk)->pt()/CLHEP::GeV;
	double mass = (trk)->m()/CLHEP::GeV;
	
	double theta = 2.*atan(exp(-eta)); //should be turned into codes
	double pCand = std::abs(pt)/sin(theta);
	//TrigMuonEFCbTrack* combinedTrk = (*tr)->CombinedTrack();
	//double mass = combinedTrk->mass();
	double iGamma = mass/pCand;
	double beta = 1./sqrt(1+iGamma*iGamma);
	//double eCand = (*tr)->e();
	if (pt > 40 && mass>40)
	  if (beta > 0 && beta<0.97)
	    result = true;
	msg() << MSG::DEBUG << " REGTEST muon pt is " << pt << " CLHEP::GeV "   
	      << " with mass "<< mass 
	      << " and beta "<< beta 
	      << " and threshold cut is 40  CLHEP::GeV" 
	      << " so hypothesis is " << (result?"true":"false") << endmsg;
      }
  }
  
  pass = result;
  return HLT::OK;
}  
