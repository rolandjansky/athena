// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// file name: MuonTrackSelector.h
// description: Take input tracks and wrap in generic track object
// author: Peter Wijeratne (paw@hep.ucl.ac.uk)

#ifndef MUONTRACKSELECTOR_H
#define MUONTRACKSELECTOR_H

// L2 tracks
#include "TrigInDetEvent/TrigInDetTrack.h"

// offline and EF
#include "Particle/TrackParticle.h"

// online EF muons
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "CaloEvent/CaloCluster.h"

#include "egammaEvent/egamma.h"

// TrigJpsiTool classes
#include "TrigJPsiTools/MuonTrackAdder.h"


class MuonTrackSelector : public MuonTrackAdder {

 public:

  MuonTrackSelector () : MuonTrackAdder() { }
  ~MuonTrackSelector () { }

  virtual void cleanup() { 

    for (int i=m_tracks.size(); i--; ){
      delete m_tracks[i];
    }     
    m_tracks.clear(); 

  }

  void muonSelector(const egamma* track, int roi) {

    if( track ) {
      double pT  = track->pt(); 
      double eta = track->eta();
      double phi = track->phi();
      double z0  = 0;//track->z0();
      double d0  = 0;//track->d0();

      double px = track->px();
      double py = track->py();
      double pz = track->pz();
      
      double charge = track->charge();
      
      double deta = 1;
      double dphi = 1;
      double dz0  = 1;
      double dd0  = 1;
      double dpT  = 1;
          
      double chi2 = 0;
      double chi2prob = 0;

      int nCscHitsEta = 0;
      int nCscHitsPhi = 0;
      int nTgcHitsEta = 0;
      int nTgcHitsPhi = 0;
      int nRpcHitsEta = 0;
      int nRpcHitsPhi = 0;
      int nMdtHits = 0;
          
      unsigned bitmap = 0;
      unsigned trackAuthor = 0;

      Wrapper::MuonTrack* muon = new Wrapper::MuonTrack(eta, phi, z0, d0, pT, 
							                                          px, py, pz, charge, chi2, chi2prob,
                                                        deta, dphi, dz0, dd0, dpT,
                                         							  nCscHitsEta, nCscHitsPhi, nTgcHitsEta, nTgcHitsPhi,
                                                        nRpcHitsEta, nRpcHitsPhi, nMdtHits,
                                                        bitmap, 0, trackAuthor, NULL, roi);
      
      if ( !muonAdder( muon ) ) delete muon;

    }
  }

  void muonSelector(const CaloCluster* track, int roi=0) {

    if( track ) {
      
      double pT  = track->pt(); 
      double eta = track->eta();
      double phi = track->phi();
      double z0  = 0;//track->z0();
      double d0  = 0;//track->d0();

      double px = track->px();
      double py = track->py();
      double pz = track->pz();

      double charge = 1.;
      
      if( pT < 0 ) {
        pT *= -1;
        charge *= -1;
      }

      double deta = 1;
      double dphi = 1;
      double dz0  = 1;
      double dd0  = 1;
      double dpT  = 1;
          
      double chi2 = 0;
      double chi2prob = 0;

      int nCscHitsEta = 0;
      int nCscHitsPhi = 0;
      int nTgcHitsEta = 0;
      int nTgcHitsPhi = 0;
      int nRpcHitsEta = 0;
      int nRpcHitsPhi = 0;
      int nMdtHits = 0;
          
      unsigned bitmap = 0;
      unsigned trackAuthor = 0;

      Wrapper::MuonTrack* muon = new Wrapper::MuonTrack(eta, phi, z0, d0, pT, 
							                                          px, py, pz, charge, chi2, chi2prob,
                                                        deta, dphi, dz0, dd0, dpT,
                                         							  nCscHitsEta, nCscHitsPhi, nTgcHitsEta, nTgcHitsPhi,
                                                        nRpcHitsEta, nRpcHitsPhi, nMdtHits,
                                                        bitmap, 0, trackAuthor, NULL, roi);
      
      if ( !muonAdder( muon ) ) delete muon;

    }
  }

  void muonSelector( const TrigMuonEFTrack* track, const Rec::TrackParticle* match=0, int roi=0){
    
    if( track ){

      //      std::cout << "ZZZ roi = " << roi << std::endl;
      
	double pT  = track->pt(); 
	double eta = track->eta();
	double phi = track->phi();
	double z0  = track->z0();
	double d0  = track->d0();

	double px = track->px();
	double py = track->py();
	double pz = track->pz();
	
	double charge = track->charge();
  
	if ( pT < 0 ) pT *= -1;

	double deta = 1;
	double dphi = 1;
	double dz0  = 1;
	double dd0  = 1;
	double dpT  = 1;
	    
	double chi2 = track->chi2();
	double chi2prob = track->chi2prob();
          
	// Check number of hits
	// NB: a spacepoint is two offline "hits", so a pixel spacepoint is really 
	// 2 "hits" and an offline SCT "hit" is really a 1D cluster, so two intersetcting
	// stereo clusters making a spacepoint are two "hits"

	int nCscHitsEta = track->NCscHitsEta(); 
	int nCscHitsPhi = track->NCscHitsPhi();  
	int nTgcHitsEta = track->NTgcHitsEta(); 
	int nTgcHitsPhi = track->NTgcHitsPhi();
	int nRpcHitsEta = track->NRpcHitsEta();
	int nRpcHitsPhi = track->NRpcHitsPhi();
	int nMdtHits = track->NMdtHits();
      
	unsigned bitmap = 0;
	unsigned trackAuthor = 0;

#if 0      
	std::cout << "\t\t\tPAW MUON track" 
		  << "\teta=" << eta  // << " +- " << (*trackitr)->params()->deta()
		  << "\tphi=" << phi  // << " +- " << (*trackitr)->params()->dphi()
		  << "\tz0="  << z0 
		  << "\tpT="  << pT // << "\t( " << 1/qoverp << ")"
		  << "\td0="  << d0
		  << "\tNCsc=" << nCscHitsEta 
		  << "\tNTgc=" << nTgcHitsEta 
		  << "\tNRpc=" << nRpcHitsEta
		  << std::endl;
     
	// Create and save Track
#endif

	Wrapper::MuonTrack* muon = new Wrapper::MuonTrack(eta, phi, z0, d0, pT, 
							  px, py, pz, charge, chi2, chi2prob,
							  deta, dphi, dz0, dd0, dpT,
							  nCscHitsEta, nCscHitsPhi, nTgcHitsEta, nTgcHitsPhi,
							  nRpcHitsEta, nRpcHitsPhi, nMdtHits,
							  bitmap, 0,
							  trackAuthor, match, roi);
      
	if ( !muonAdder( muon ) ) delete muon;

    }

  }

  void muonSelector( const Rec::TrackParticle* track ){
    
    if( track ){

	const Trk::Perigee* measPer = track->measuredPerigee();
      
	double pT  = measPer->pT(); 
	double eta = measPer->eta();
	double phi = measPer->parameters()[Trk::phi0];
	double z0  = measPer->parameters()[Trk::z0];
	double d0  = measPer->parameters()[Trk::d0];
  
	double px = track->px();
	double py = track->py();
	double pz = track->pz();
	
	double charge = track->charge();

	if ( measPer->parameters()[Trk::qOverP]<0 ) pT *= -1;

	double deta = 1;
	double dphi = 1;
	double dz0  = 1;
	double dd0  = 1;
	double dpT  = 1;
	    
	double chi2 = 0;
          
	// Check number of hits
	// NB: a spacepoint is two offline "hits", so a pixel spacepoint is really 
	// 2 "hits" and an offline SCT "hit" is really a 1D cluster, so two intersetcting
	// stereo clusters making a spacepoint are two "hits"
	const Trk::TrackSummary* summary = track->trackSummary();
	int nBlayerHits = 2*summary->get(Trk::numberOfInnermostPixelLayerHits);
	int nPixelHits  = 2*summary->get(Trk::numberOfPixelHits);  
	int nSctHits    = summary->get(Trk::numberOfSCTHits); 
	int nStrawHits  = summary->get(Trk::numberOfTRTHits);
	int nTrHits     = summary->get(Trk::numberOfTRTHighThresholdHits);
	
	int nSiHits     = nPixelHits + nSctHits;
      
	unsigned bitmap = 0;
      
	int fitter = track->info().trackFitter();
	std::string dumpinfo = track->info().dumpInfo();
      
	int trackAuthor = -1; 
	if ( fitter>0 && fitter<Trk::TrackInfo::NumberOfTrackFitters ) {
	  if      ( dumpinfo.find("TRTStandalone")!=std::string::npos)        trackAuthor = 2;
	  else if ( dumpinfo.find("TRTSeededTrackFinder")!=std::string::npos) trackAuthor = 1;
	  else                                                                trackAuthor = 0;
	}

#if 0      
	std::cout << "\t\t\tPAW MUON track" 
		  << "\teta=" << eta  // << " +- " << (*trackitr)->params()->deta()
		  << "\tphi=" << phi  // << " +- " << (*trackitr)->params()->dphi()
		  << "\tz0="  << z0 
		  << "\tpT="  << pT // << "\t( " << 1/qoverp << ")"
		  << "\td0="  << d0
		  << "\tNsi=" << nSiHits 
		  << "\tNtrt=" << nTrHits 
		  << "\tNstr=" << nStrawHits
		  << "\tauthor=" << trackAuthor
		  << std::endl;
#endif	
	// Create and save Track

	Wrapper::MuonTrack* muon = new Wrapper::MuonTrack(eta, phi, z0, d0, pT, 
							  px, py, pz, charge, chi2,
							  deta, dphi, dz0, dd0, dpT,
							  nBlayerHits, nPixelHits, nSctHits, nSiHits,
							  nStrawHits, nTrHits, bitmap, 0,
							  trackAuthor);
      
	if ( !muonAdder( muon ) ) delete muon;

    }
    
  }

  void muonSelector( const TrigInDetTrack* track ) {     
    // do the track extraction stuff here....
    if ( track ) { 

	double eta    = track->param()->eta();
	double phi    = track->param()->phi0();
	double z0     = track->param()->z0(); 
	double pT     = track->param()->pT(); 
	double d0     = track->param()->a0();

	double deta    = track->param()->eeta();
	double dphi    = track->param()->ephi0();
	double dz0     = track->param()->ez0(); 
	double dpT     = track->param()->epT(); 
	double dd0     = track->param()->ea0();

	int   algoid  = track->algorithmId(); 	      

	int nBlayerHits = (track->HitPattern() & 0x1);
	int nPixelHits  = 2 * track->NPixelSpacePoints();  // NB: for comparison with offline 
	int nSctHits    = 2 * track->NSCT_SpacePoints();   //     a spacepoint is 2 "hits"
	int nStrawHits  =  track->NStrawHits();
	int nTrHits     =  track->NTRHits();
	           
	int nSiHits     = nPixelHits + nSctHits;

	unsigned hitPattern = track->HitPattern();
	unsigned multiPattern = 0;

	double chi2    = track->chi2();
	// The DoF information is not available in LVL2 track EDM

	//	if ( (*trackitr)->siSpacePoints() ) { 
	//	    NSiHits = (*trackitr)->siSpacePoints()->size();
	//	    if ( (*trackitr)->siSpacePoints()->at(0)->layer()==0 ) NBLayerHits=1; 
	//	}

	//	std::cout << "testing trigger track " << (*trackitr) << std::endl;

#if 0
	std::cout << "\t\t\tSUTT ID track" 
		  << "\teta=" << eta  // << " +- " << (*trackitr)->params()->deta()
		  << "\tphi=" << phi  // << " +- " << (*trackitr)->params()->dphi()
		  << "\tz0="  << z0 
		  << "\tpT="  << pT // << "\t( " << 1/qoverp << ")"
		  << "\td0="  << d0
		  << "\tNtrt=" << nTrHits 
		  << "\tNstr=" << nStrawHits 
		  << "\tid="  << algoid 
		  << "\thp=0x" << std::hex << track->HitPattern() << std::dec 
		  << std::endl;
#endif

	Wrapper::MuonTrack* muon = new Wrapper::MuonTrack(  eta,  phi,  z0,  d0,  pT, chi2, 
							    deta, dphi, dz0, dd0, dpT, 
							    nBlayerHits, nPixelHits, nSctHits, nSiHits, 
							    nStrawHits, nTrHits, 
							    hitPattern, multiPattern, 
							    algoid ) ; 

	//	std::cout << "SUTT ID track " << *t << "\t0x" << std::hex << track->HitPattern() << std::dec << std::endl;

	if ( !muonAdder( muon ) ) delete muon;

    }
  }

};

#endif
