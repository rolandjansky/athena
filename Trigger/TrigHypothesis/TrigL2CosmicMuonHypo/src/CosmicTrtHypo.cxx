/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ITHistSvc.h"
#include "TrigL2CosmicMuonHypo/CosmicTrtHypo.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "StoreGate/StoreGateSvc.h"

using namespace std;


CosmicTrtHypo::CosmicTrtHypo(const std::string& name, ISvcLocator* svcloc) 
  : HLT::HypoAlgo(name, svcloc) {
  declareProperty("NTrtSegs",mNTrtSegs=1);
  declareProperty("NTrthitsCut",  mNTrthitsCut=10);
  declareProperty("TrtSegD0Cut",  mTrtSegD0Cut=100000.0);
    // Monitoring
  declareMonitoredVariable("numtracks_hypo",m_ntracksHypo);
  
  declareMonitoredStdContainer("nStraw_TRT_hypo", m_nStraw_TRT_hypo);
  declareMonitoredStdContainer("nTR_TRT_hypo", m_nTR_TRT_hypo);
  declareMonitoredStdContainer("AcceptedTrack_a0", mAcceptedTrack_a0);
  declareMonitoredStdContainer("AcceptedTrack_pt", mAcceptedTrack_pt);
  declareMonitoredStdContainer("AcceptedTrack_phi0", mAcceptedTrack_phi0);
  declareMonitoredStdContainer("AcceptedTrack_z0", mAcceptedTrack_z0);
  declareMonitoredStdContainer("AcceptedTrack_eta", mAcceptedTrack_eta);
    
}

CosmicTrtHypo::~CosmicTrtHypo() {
}

HLT::ErrorCode CosmicTrtHypo::hltInitialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "initialize()" << endreq;

  m_ntracksHypo = 0;

  StatusCode sc2 = service("StoreGateSvc", mStoreGate);
  if (sc2.isFailure()) {
    log << MSG::FATAL << "cannot get StoreGateSvc" << endreq;
    return HLT::ERROR;
  }


  log << MSG::INFO << "Parameters for CosmicTrtHypo:" << name() << endreq;
  log << MSG::INFO << "NTrtSegs   : " <<mNTrtSegs << endreq;
  log << MSG::INFO << "NTrthitsCut    : " <<  mNTrthitsCut << endreq;
  log << MSG::INFO << "TrtSegD0Cut    : " <<  mTrtSegD0Cut << endreq;
  
  log << MSG::INFO << "CosmicTrtHypo initialized successfully" << endreq;

  return HLT::OK;
}


HLT::ErrorCode CosmicTrtHypo::hltFinalize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "finalize()" << endreq;
  return HLT::OK;
}

HLT::ErrorCode CosmicTrtHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass){
  MsgStream log(msgSvc(), name());
  int output_level = msgSvc()->outputLevel();
  bool passed = false;


  if (output_level <= MSG::DEBUG) {
    log << MSG::DEBUG << " in  hltExecute "<< endreq;
  }

  // clearEvent
  m_nStraw_TRT_hypo.clear();
  m_nTR_TRT_hypo.clear();
  mAcceptedTrack_a0.clear();
  mAcceptedTrack_pt.clear();
  mAcceptedTrack_phi0.clear();
  mAcceptedTrack_z0.clear();
  mAcceptedTrack_eta.clear();


  // retrieve vector of InDetTracks from TE 
  std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections;
  HLT::ErrorCode status2 = getFeatures(inputTE, vectorOfTrackCollections);
  if(status2 != HLT::OK) 
    {
      msg() << MSG::ERROR << " Cannot get InDetTrackCollections " << endreq;
      return status2;
    } 
  else 
  {

   msg() << MSG::VERBOSE << "execHLTAlgorithm: got vectorOfTracksCollections of size "<<
	vectorOfTrackCollections.size()<< endreq;
      if (vectorOfTrackCollections.size()==0) return HLT::OK;

      std::vector<const TrigInDetTrackCollection*>::iterator theTrackColl = vectorOfTrackCollections.begin();
      std::vector<TrigInDetTrack* > myCollection;

      TrigInDetTrackCollection::const_iterator it = (*theTrackColl)->begin(), e = (*theTrackColl)->end();

      for (; it != e; ++it)
	myCollection.push_back(*it);
      int nsegs=0;
      
      for (int i = 0; i < (int)myCollection.size(); ++i) 
	{  
	  const TrigInDetTrack * track = myCollection[i];
	  const TrigInDetTrackFitPar* par = track->param();

	  log << MSG::DEBUG << " track "<< i <<" has "<< track->NStrawHits() << " Trt hits "<<endreq;

	  if((track->NStrawHits())>mNTrthitsCut&&fabs(par->a0())<mTrtSegD0Cut )  {
	    nsegs ++;

	    m_nStraw_TRT_hypo.push_back(track->NStrawHits());
	    m_nTR_TRT_hypo.push_back(track->NTRHits());
	    mAcceptedTrack_a0.push_back(par->a0());
	    mAcceptedTrack_pt.push_back(par->pT());
	    mAcceptedTrack_phi0.push_back(par->phi0());
	    mAcceptedTrack_z0.push_back(par->z0());
	    mAcceptedTrack_eta.push_back(par->eta());
	    
	  }
 	}
      
      //Monitoring
      
      m_ntracksHypo = nsegs; 

	if (nsegs>=mNTrtSegs) {
	  passed = true;
	} else {
	  passed = false;
	}
      
      
      if (output_level <= MSG::DEBUG) {
	log << MSG::DEBUG << "Passed or not: " << passed << endreq;
      }
      
      if (passed)
	pass = true;
      else
	pass = false;
      return HLT::OK;
      
      
  }
  
  
}

