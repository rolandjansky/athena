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

#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigParticle/TrigTau.h"

#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigT2Tau/T2TauMatchingTool.h"

T2TauMatchingTool::T2TauMatchingTool(const std::string & type, const std::string & name, 
                   const IInterface* parent): AthAlgTool(type, name, parent){
  declareInterface<IAlgToolTau>(this);
/** PT Cut for tracks */
  declareProperty("TrackPTCut",m_pTCut = 1. *CLHEP::GeV);
/** Cone size for track matching */
  declareProperty("TrackMatchConeCut",m_coneCut = 0.3);
}
 
T2TauMatchingTool::~T2TauMatchingTool(){
}
 
StatusCode T2TauMatchingTool::initialize(){
  ATH_MSG_DEBUG( "in initialize()" );
  ATH_MSG_DEBUG( "REGTEST: Will consider tracks with pT > " << m_pTCut 
                 << " within dR <" << m_coneCut << " from cluster " );
  return StatusCode::SUCCESS;
}
 
StatusCode T2TauMatchingTool::finalize(){
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}

StatusCode T2TauMatchingTool::execute(const TrigTauCluster *pTrigTauCluster,
				      const TrigTauTracksInfo * pTracks,
				      TrigTau& pTrigTau){
  if( pTracks==0 || pTracks->trackCollection() == 0 || pTrigTauCluster == 0  )
    {
      ATH_MSG_DEBUG( " Pointer to cluster or tracks is zero, nothing to match" );
      return StatusCode::SUCCESS;
    }

  if( pTracks->trackCollection()->begin() == pTracks->trackCollection()->end() ) 
   {
     ATH_MSG_DEBUG( " Empty track container, nothing to match" );
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

    ATH_MSG_DEBUG( "PT of the track:" <<(*itr)->param()->pT() 
                   <<" (track algo=" << (*itr)->algorithmId() << ")");

    TrigInDetTrackHelper trackHelper((*itr)->param());
    double phic=0, etac=0;
	      
    static const float RCAL =  1470.;
    static const float ZCAL = 3800.;

    trackHelper.extrapolate(RCAL, ZCAL, phic,etac);
    
    ATH_MSG_DEBUG( "extrapolated track eta/phi=" << etac <<"/"<< phic);
    
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
  
  ATH_MSG_DEBUG( "Number of accepted tracks" << ntracks );
  
  pTrigTau.setZvtx((*itrHigh)->param()->z0());
  pTrigTau.setErr_Zvtx((*itrHigh)->param()->ez0());
  pTrigTau.setNMatchedTracks(ntracks);
    
  return StatusCode::SUCCESS;
}

