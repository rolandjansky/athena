/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauMatchingTool.cxx
// PACKAGE:  Trigger/TrigAlgoritms/TrigT2Tau
// 
// AUTHOR:   M.P. Casado
// 
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
 
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigParticle/TrigTau.h"

#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigT2Tau/T2TauMatchingTool.h"

T2TauMatchingTool::T2TauMatchingTool(const std::string & type, const std::string & name, 
                   const IInterface* parent): AlgTool(type, name, parent){
  declareInterface<IAlgToolTau>(this);
/** PT Cut for tracks */
  declareProperty("TrackPTCut",m_pTCut = 1. *CLHEP::GeV);
/** Cone size for track matching */
  declareProperty("TrackMatchConeCut",m_coneCut = 0.3);
}
 
T2TauMatchingTool::~T2TauMatchingTool(){
}
 
StatusCode T2TauMatchingTool::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in initialize()" << endreq;
  log << MSG::DEBUG << "REGTEST: Will consider tracks with pT > " << m_pTCut 
      << " within dR <" << m_coneCut << " from cluster " << endreq;
  return StatusCode::SUCCESS;
}
 
StatusCode T2TauMatchingTool::finalize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode T2TauMatchingTool::execute(const TrigTauCluster *pTrigTauCluster,
				      const TrigTauTracksInfo * pTracks,
				      TrigTau& pTrigTau){
  MsgStream log(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel(name());

  if( pTracks==0 || pTracks->trackCollection() == 0 || pTrigTauCluster == 0  )
    {
      if( outputLevel <=  MSG::DEBUG )
	log << MSG::DEBUG << " Pointer to cluster or tracks is zero, nothing to match" << endreq;
      return StatusCode::SUCCESS;
    }

  if( pTracks->trackCollection()->begin() == pTracks->trackCollection()->end() ) 
   {
      log << MSG::DEBUG << " Empty track container, nothing to match" << endreq;
      return StatusCode::SUCCESS;
   }

  double eta = pTrigTauCluster->eta();
  double phi = pTrigTauCluster->phi();
   
  int ntracks = 0;
  double pThigh= -999.;
  
  TrigInDetTrackCollection::const_iterator itrHigh = pTracks->trackCollection()->begin();
   
  TrigInDetTrackCollection::const_iterator itr = pTracks->trackCollection()->begin();
  TrigInDetTrackCollection::const_iterator itr_last = pTracks->trackCollection()->end();
   
  for (;itr!=itr_last;++itr) {

    if (outputLevel <= MSG::DEBUG) 
      {
	unsigned int algoId = (*itr)->algorithmId();
	log << MSG::DEBUG << "PT of the track:" <<(*itr)->param()->pT() 
	    <<" (track algo=" << algoId << ")"<<endreq;
      }

    TrigInDetTrackHelper trackHelper((*itr)->param());
    double phic=0, etac=0;
	      
    static const float RCAL =  1470.;
    static const float ZCAL = 3800.;

    trackHelper.extrapolate(RCAL, ZCAL, phic,etac);
    
    if (outputLevel <= MSG::DEBUG) 
      log << MSG::DEBUG << "extrapolated track eta/phi=" << etac <<"/"<< phic<< endreq;
    
    // Deal with angle diferences greater than Pi
    double TDphi =  phic - phi;
    TDphi = (fabs(TDphi) < M_PI) ? fabs(TDphi) : fabs(2*M_PI -  TDphi);
    
    double dEta = etac-eta;

    double trk_clus_dist = sqrt(dEta*dEta + TDphi*TDphi);
    
    if ((fabs( (*itr)->param()->pT()) > m_pTCut ) &&
         (trk_clus_dist < m_coneCut)) {
      ++ntracks;

      if (pThigh < (*itr)->param()->pT() ) {
        pThigh = (*itr)->param()->pT();
        itrHigh = itr;
      }

    }
    
  }
  
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Number of accepted tracks" << ntracks << endreq;  
  
  pTrigTau.setZvtx((*itrHigh)->param()->z0());
  pTrigTau.setErr_Zvtx((*itrHigh)->param()->ez0());
  pTrigTau.setNMatchedTracks(ntracks);
    
  return StatusCode::SUCCESS;
}

