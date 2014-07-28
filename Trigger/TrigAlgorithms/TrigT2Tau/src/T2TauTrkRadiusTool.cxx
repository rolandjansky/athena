/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauTrkRadiusTool.cxx
// PACKAGE:  Trigger/TrigAlgoritms/TrigT2Tau
// 
// AUTHOR:   Olya Igonkina
// 20120215, ccuenca, |dZ0| selection implemented and dPhi bug fixed
// 
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
 
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigParticle/TrigTau.h"

#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigT2Tau/T2TauTrkRadiusTool.h"

T2TauTrkRadiusTool::T2TauTrkRadiusTool(const std::string & type, const std::string & name, 
                   const IInterface* parent): AlgTool(type, name, parent){
  declareInterface<IAlgToolTau>(this);
  /** PT Cut for tracks */
  declareProperty("TrackPTCut",        m_pTCut   = 1.0 * CLHEP::GeV);
  /** Cone size for trkAvgDist */
  declareProperty("TrackMatchConeCut", m_coneCut = 0.3);
  /** dZ0 selection for tracks */
  declareProperty("TrackdZ0Max",       m_dZ0Max  = -1.0); //disabled by default

}
 
T2TauTrkRadiusTool::~T2TauTrkRadiusTool(){
}
 

StatusCode T2TauTrkRadiusTool::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "in initialize()" << endreq;
  log << MSG::DEBUG << "REGTEST: Will consider tracks with pT>" << m_pTCut 
      << " within dR<" << m_coneCut << "from cluster" 
      << endreq;
  if ( m_dZ0Max > 0 ) log << MSG::DEBUG << "REGTEST: and with |DeltaZ0|<" << m_dZ0Max << endreq;
  else                log << MSG::DEBUG << "REGTEST: and with no |DeltaZ0| selection" << endreq;

  return StatusCode::SUCCESS;
}

 
StatusCode T2TauTrkRadiusTool::finalize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode T2TauTrkRadiusTool::execute(const TrigTauCluster *pTrigTauCluster,
				      const TrigTauTracksInfo * pTracks,
				      TrigTau& pTrigTau){
  MsgStream log(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel(name());


  //Get tracks
  if( pTracks==0 || pTracks->trackCollection() == 0 || pTrigTauCluster == 0  ) {
    if( outputLevel <=  MSG::DEBUG )
        log << MSG::DEBUG << " Pointer to cluster or tracks is zero" << endreq;
    return StatusCode::SUCCESS;
  }

  if( pTracks->trackCollection()->begin() == pTracks->trackCollection()->end() ) {
    log << MSG::DEBUG << " Empty track container" << endreq;
    return StatusCode::SUCCESS;
  }


  //calo cluster coords
  double eta = pTrigTauCluster->eta();
  double phi = pTrigTauCluster->phi();
   
   

  //find leading track Z0;
  TrigInDetTrackCollection::const_iterator itr = pTracks->trackCollection()->begin();
  TrigInDetTrackCollection::const_iterator itr_last = pTracks->trackCollection()->end();
  double ldTrkPt = -999.9;
  double ldTrkZ0 = -999.9;//Z0 should be in [-250,250] CLHEP::mm
  for (;itr!=itr_last;++itr) {
    float pt = fabs( (*itr)->param()->pT());
    if ( pt > ldTrkPt) {
      ldTrkPt = pt;
      ldTrkZ0 = (*itr)->param()->z0();
    }
  }
  if (outputLevel <= MSG::DEBUG) 
      log << MSG::DEBUG 
          << "REGTEST: found leading track with pt=" << ldTrkPt 
          << " and Z0=" << ldTrkZ0 << endreq;


  //calculate track radius
  float dRPtWeighted = 0.0;
  float scalarPtSum  = 0.0;
  itr = pTracks->trackCollection()->begin();//re-start the loop
  for (;itr!=itr_last;++itr) {

    if (outputLevel <= MSG::DEBUG) {
      unsigned int algoId = (*itr)->algorithmId();
      log << MSG::DEBUG 
          << "REGTEST: PT of the track:" <<(*itr)->param()->pT() 
          //<< ", Z0="  << (*itr)->param()->z0()
          //<< ", dZ0=" << (*itr)->param()->z0() - ldTrkZ0
          //<< ", dR="  << dr
          <<" (track algo=" << algoId << ")" 
          <<endreq;
    }
    
    //ptMin selection
    float pt = fabs( (*itr)->param()->pT());
    if ( pt < m_pTCut )
        continue;
    
    //|deltaZ0| selection (note that ldtrk pt will have to be above the m_pTCut threshold
    if ( m_dZ0Max >= 0.0 ) { //if the selection is enabled
      float dz0 = (*itr)->param()->z0() - ldTrkZ0;
      if ( fabs( dz0) > m_dZ0Max) 
          continue;
    }
      
    // Phi is defined [-CLHEP::pi,CLHEP::pi]. dPhi to be used only for dR calculation: no abs!
    double dPhi = (*itr)->param()->phi0() - phi; while (dPhi >= M_PI ) dPhi -= 2*M_PI; while (dPhi < -1.0*M_PI ) dPhi += 2*M_PI;
    double dEta = (*itr)->param()->eta()  - eta;
    double dr   = sqrt( dEta*dEta + dPhi*dPhi );
    if ( (dr < m_coneCut) ) {
      //log << MSG::DEBUG << "REGTEST: track accepted!!" << endreq;
      dRPtWeighted += pt * dr;
      scalarPtSum  += pt;
    }
    
  }
  
  //trkAvgDist
  if (scalarPtSum > 0.01 )    pTrigTau.setTrkAvgDist(dRPtWeighted / scalarPtSum);
  else                        pTrigTau.setTrkAvgDist(-111.);

  if (outputLevel <= MSG::DEBUG) 
      log << MSG::DEBUG 
          << "REGTEST: scalarPtSum=" << scalarPtSum
          << " trkAvgDist=" << pTrigTau.trkAvgDist() 
          << endreq;  
  
  //big success!!
  return StatusCode::SUCCESS;
}

