// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file JpsiTrackSelector.h **/
/** @author Peter Wijeratne (paw@hep.ucl.ac.uk) **/

#ifndef TrigIDJpsiMonitoring_JpsiTrackSelector_H
#define TrigIDJpsiMonitoring_JpsiTrackSelector_H

#include "TrigInDetAnalysis/TIDAVertex.h"

#include "TrigInDetAnalysisUtils/TrigTrackSelector.h"

#include "TrigIDJpsiMonitoring/JpsiTrack.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"

#include "Particle/TrackParticle.h"

class JpsiTrackSelector : public TrigTrackSelector {

 public:

  JpsiTrackSelector( TrackFilter* selector) : TrigTrackSelector( selector ) { }
 
  void addJpsi(std::vector<DiMuonTool*>& diMuon) {

    std::vector<DiMuonTool*>::iterator countZero;

    for (countZero=diMuon.begin();countZero!=diMuon.end();countZero++){

      //std::cout << "addJpsi..." << std::endl;
      const Wrapper::MuonTrack* probeTrack = (*countZero)->ProbedTrack();
      //std::cout << "...probe done." << std::endl;
      /*std::cout << "...probe done..." << std::endl;
      const Wrapper::MuonTrack* tagTrack = (*countZero)->TaggedTrack();
      std::cout << "...tag done." << std::endl;*/

      if( probeTrack ) {

        double pT  = probeTrack->pt(); 
        double eta = probeTrack->eta();
        double phi = probeTrack->phi();
        double z0  = probeTrack->z0();
        double d0  = probeTrack->a0();
	
        if ( pT < 0 ) pT *= -1;
	
        double deta = 1;
        double dphi = 1;
        double dz0  = 1;
        double dd0  = 1;
        double dpT  = 1;
	    
        double chi2 = probeTrack->chi2();
          
        // Check number of hits
        // NB: a spacepoint is two offline "hits", so a pixel spacepoint is really 
        // 2 "hits" and an offline SCT "hit" is really a 1D cluster, so two intersetcting
        // stereo clusters making a spacepoint are two "hits"
	
        /*	int nCscHitsEta = probeTrack->bLayerHits(); 
        int nCscHitsPhi = probeTrack->pixelHits();  
        int nTgcHitsEta = probeTrack->sctHits(); 
        int nTgcHitsPhi = probeTrack->siHits();
        int nRpcHitsEta = probeTrack->strawHits();
        int nRpcHitsPhi = probeTrack->trHits();*/
      
        unsigned bitmap = 0;
        unsigned trackAuthor = 0;

        const Rec::TrackParticle* match = probeTrack->match();

        int roi = probeTrack->roiNum();
        /*      
        std::cout << "\t\t\tPAW JPSI track" 
        	  << "\teta=" << eta  // << " +- " << (*trackitr)->params()->deta()
        	  << "\tphi=" << phi  // << " +- " << (*trackitr)->params()->dphi()
        	  << "\tz0="  << z0 
        	  << "\tpT="  << pT // << "\t( " << 1/qoverp << ")"
        	  << "\td0="  << d0
        	  << "\tNCsc=" << nCscHitsEta 
        	  << "\tNTgc=" << nTgcHitsEta 
        	  << "\tNRpc=" << nRpcHitsEta
        	  << std::endl;
        */
        // Create and save Track

        int nRpcHitsPhi = probeTrack->nRpcHitsPhi();
        int nTgcHitsPhi = probeTrack->nTgcHitsPhi();
        int nCscHitsPhi = probeTrack->nCscHitsPhi();
        int nRpcHitsEta = probeTrack->nRpcHitsEta();
        int nTgcHitsEta = probeTrack->nTgcHitsEta();
        int nCscHitsEta = probeTrack->nCscHitsEta();

        TIDA::JpsiTrack* jpsi = new TIDA::JpsiTrack(TIDA::Track(eta, phi, z0, d0, pT, chi2,
        											       deta, dphi, dz0, dd0, dpT,
        											       nCscHitsEta, nCscHitsPhi, nTgcHitsEta, nTgcHitsPhi,
        											       nRpcHitsEta, nRpcHitsPhi, bitmap, 0,
        											       trackAuthor), match, *countZero, roi );
	
        if ( !addTrack( jpsi ) )  delete jpsi; 
	
      } // for(countZero = diMuon.begin(); countZero != diMuon.end(); countZero++)
    } // if( probeTrack )
  } // void addJpsi(std::vector<DiMuonTool*>& diMuon) 

  virtual void clear() { 
    //std::cout << "clearing now..." << std::endl;
    //std::cout << "JpsiTrackSelector clear -- m_tracks size = " << m_tracks.size() << std::endl;
    for ( unsigned i=m_tracks.size() ; i-- ; ) { 
      //std::cout << "m_tracks[" << i << "] = " << m_tracks[i] << std::endl;
      delete m_tracks[i];
    }
    m_tracks.clear();
    //TrigTrackSelector::clear();
  }
};

#endif
