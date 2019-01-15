/* emacs: this is -*- c++ -*- */
/**
 **     @file    Converter.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:39 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysis_Converter_H
#define TrigInDetAnalysis_Converter_H


#include "TMath.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "muonEvent/MuonContainer.h"

#include "TrigInDetAnalysis/Track.h"


class Converter { 
  
 public:
  
  Converter() {
    m_beamX = 0;
    m_beamY = 0;
    m_beamZ = 0;
  }
    
  Converter(double x, double y, double z) {
    m_beamX = x;
    m_beamY = y;
    m_beamZ = z;
  }
  
  
  // Method adding a track
  void addTrack(TIDA::Track* t) {
    m_tracks.push_back(t);
  }
  
  // get the tracks   
  const std::vector<TIDA::Track*>& tracks() const {return m_tracks;}
  
  // clear tracks 
  void clear() {m_tracks.clear();} 

  // Method converting TrigInDetTrack objects
  void selectTracks(const TrigInDetTrackCollection* trigtracks) {
    
    TrigInDetTrackCollection::const_iterator  trackitr = trigtracks->begin();
    TrigInDetTrackCollection::const_iterator  trackend = trigtracks->end();

    while ( trackitr!=trackend ) { 

	double eta    = (*trackitr)->param()->eta();
	double phi    = (*trackitr)->param()->phi0();
	double z0     = (*trackitr)->param()->z0(); 
	double pT     = (*trackitr)->param()->pT(); 
	double d0     = (*trackitr)->param()->a0();
	//double theta  = 2*atan2(exp(-(*trackitr)->param()->eta()),1);

	double deta    = (*trackitr)->param()->eeta();
	double dphi    = (*trackitr)->param()->ephi0();
	double dz0     = (*trackitr)->param()->ez0(); 
	double dpT     = (*trackitr)->param()->epT(); 
	double dd0     = (*trackitr)->param()->ea0();

	int   algoid  = (*trackitr)->algorithmId(); 	      

	int nBlayerHits = ((*trackitr)->HitPattern() & 0x1);
	int nPixelHits  = 2*(*trackitr)->NPixelSpacePoints();  // NB: for comparison with offline 
	int nSctHits    = 2*(*trackitr)->NSCT_SpacePoints();   //     a spacepoint is 2 "hits"
	int nStrawHits  = (*trackitr)->NStrawHits();
	int nTrHits     = (*trackitr)->NTRHits();
	           
	int nSiHits     = nPixelHits + nSctHits;
	bool expectBL   = false;                               //not available with TrigInDetTrack

	unsigned hitPattern = (*trackitr)->HitPattern();
	unsigned multiPattern = 0;

	double chi2    = (*trackitr)->chi2();
	double dof     = 0;

	// Shift d0 and z0 according to beam position
	ipCorr(d0, z0, d0, z0, phi, eta, pT);
	
	// Create and save Track  
	TIDA::Track* t = new TIDA::Track(eta,  phi,  z0,  d0,  pT, chi2, dof, 
								   deta, dphi, dz0, dd0, dpT, 
								   nBlayerHits, nPixelHits, nSctHits, nSiHits, 
								   nStrawHits, nTrHits, hitPattern, multiPattern, 
								   algoid,
								   expectBL ) ; 

	addTrack(t);
	trackitr++; 
    }
  }


  // Method converting TrackParticle objects
  void selectTracks( const Rec::TrackParticleContainer* trigtracks ) { 
    
    Rec::TrackParticleContainer::const_iterator  trackitr = trigtracks->begin();
    Rec::TrackParticleContainer::const_iterator  trackend = trigtracks->end();
    
    static int hpmap[20] = { 0, 1, 2,  7, 8, 9,  3, 4, 5, 6, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    while ( trackitr!=trackend ) { 
	    
      const Trk::MeasuredPerigee* measPer = (*trackitr)->measuredPerigee();

      CLHEP::HepVector perigeeParams = measPer->parameters();    
      double pT    = measPer->pT(); 
      double eta   = measPer->eta();
      double phi   = perigeeParams[Trk::phi0];
      double z0    = perigeeParams[Trk::z0];
      double d0    = perigeeParams[Trk::d0];
      //double theta = perigeeParams[Trk::theta];
	
      double deta = 1;
      double dphi = 1;
      double dz0  = 1;
      double dd0  = 1;
      double dpT  = 1;	  

      // Check number of hits
      // NB: a spacepoint is two offline "hits", so a pixel spacepoint is really 
      // 2 "hits" and an offline SCT "hit" is really a 1D cluster, so two intersetcting
      // stereo clusters making a spacepoint are two "hits"
      const Trk::TrackSummary *summary = (*trackitr)->trackSummary();
      int nBlayerHits = 2*summary->get(Trk::numberOfBLayerHits); 
      int nPixelHits  = 2*summary->get(Trk::numberOfPixelHits);  
      int nSctHits    = summary->get(Trk::numberOfSCTHits); 
      int nStrawHits  = summary->get(Trk::numberOfTRTHits);
      int nTrHits     = summary->get(Trk::numberOfTRTHighThresholdHits);
      int nSiHits     = nPixelHits + nSctHits;
      bool expectBL   = summary->get(Trk::expectBLayerHit);

      const Trk::FitQuality *quality   = (*trackitr)->fitQuality();
      double chi2 = quality->chiSquared();
      double dof  = quality->numberDoF();

      unsigned bitmap = 0;
      
      for ( int ih=0 ; ih<20 ; ih++ ) {
	if ( summary->isHit(Trk::DetectorType(ih)) ) bitmap |= ( 1<<hpmap[ih] ); 	
      }

      // Shift d0 and z0 according to beam position
      ipCorr(d0, z0, d0, z0, phi, eta, pT);

      // Create and save Track  
      TIDA::Track* t = new TIDA::Track(eta, phi, z0, d0, pT, chi2, dof, 
								 deta, dphi, dz0, dd0, dpT,
								 nBlayerHits, nPixelHits, nSctHits, nSiHits,
								 nStrawHits, nTrHits, bitmap, 0,
								 -1,
								 expectBL) ;
  
      addTrack(t);
      trackitr++;
    }
    
  }
  
  // Method converting TrackParticle objects
  void selectTracks( const Analysis::MuonContainer* muontracks ) {
    
    static int hpmap[20] = { 0, 1, 2,  7, 8, 9,  3, 4, 5, 6, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
    
    Analysis::MuonContainer::const_iterator muonitr = muontracks->begin();
    Analysis::MuonContainer::const_iterator muonend = muontracks->end();
    
    while(muonitr!=muonend) { 
      
      bool hasInDet = (*muonitr)->hasInDetTrackParticle();
      const Rec::TrackParticle *trackitr=NULL; 
      if(hasInDet) trackitr=(*muonitr)->inDetTrackParticle();
      muonitr++;
      if(!hasInDet) continue;
      
      const Trk::MeasuredPerigee* measPer = trackitr->measuredPerigee();
      
      CLHEP::HepVector perigeeParams = measPer->parameters();    
      double pT    = measPer->pT(); 
      double eta   = measPer->eta();
      double phi   = perigeeParams[Trk::phi0];
      double z0    = perigeeParams[Trk::z0];
      double d0    = perigeeParams[Trk::d0];
      //double theta = perigeeParams[Trk::theta];
      
      double deta = 1;
      double dphi = 1;
      double dz0  = 1;
      double dd0  = 1;
      double dpT  = 1;	  
      
      // Check number of hits
      // NB: a spacepoint is two offline "hits", so a pixel spacepoint is really 
      // 2 "hits" and an offline SCT "hit" is really a 1D cluster, so two intersetcting
      // stereo clusters making a spacepoint are two "hits"
      const Trk::TrackSummary *summary = trackitr->trackSummary();
      int nBlayerHits = 2*summary->get(Trk::numberOfBLayerHits); 
      int nPixelHits  = 2*summary->get(Trk::numberOfPixelHits);  
      int nSctHits    = summary->get(Trk::numberOfSCTHits); 
      int nStrawHits  = summary->get(Trk::numberOfTRTHits);
      int nTrHits     = summary->get(Trk::numberOfTRTHighThresholdHits);
      
      int nSiHits     = nPixelHits + nSctHits;
      
      const Trk::FitQuality *quality   = trackitr->fitQuality();
      double chi2 = quality->chiSquared();
      double dof  = quality->quality->numberDoF();

      unsigned bitmap = 0;
      
      for ( int ih=0 ; ih<20 ; ih++ ) {
	if ( summary->isHit(Trk::DetectorType(ih)) ) bitmap |= ( 1<<hpmap[ih] ); 	
      }
      
      // Shift d0 and z0 according to beam position
      ipCorr(d0, z0, d0, z0, phi, eta, pT);
      
      // Create and save Track  
      TIDA::Track* t = new TIDA::Track(eta, phi, z0, d0, pT, chi2, dof,
								 deta, dphi, dz0, dd0, dpT,
								 nBlayerHits, nPixelHits, nSctHits, nSiHits,
								 nStrawHits, nTrHits, bitmap, 0,
								 -1) ;
      
      addTrack(t);
    }
    
  }
  
 protected:
  
  double phiCorr(double phi){
    
    if (phi < -TMath::Pi()) phi += 2*TMath::Pi();
    if (phi >  TMath::Pi()) phi -= 2*TMath::Pi();
    
    return phi;
  }


  void ipCorr(double d0, double z0, double& d0c, double& z0c, double phi0, double eta, double pt) {
    
    double sn  = sin(phi0);
    double cs  = cos(phi0);
    double sd0 = (d0 != 0 ? d0/fabs(d0) : 1);
    double spt = (pt != 0 ? pt/fabs(pt) : 1);
    
    if (fabs(pt) >= 1*CLHEP::TeV) {

      d0c = d0 + m_beamX*sn - m_beamY*cs;

    } else {
      
      double rc = fabs(pt)*15.0/(9.0*1.042);
      
      double xc = (fabs(d0)-spt*sd0*rc)*cos(phi0+M_PI/2*sd0) - m_beamX;
      double yc = (fabs(d0)-spt*sd0*rc)*sin(phi0+M_PI/2*sd0) - m_beamY;
      
      double newphi;
      double xd01,yd01,xd02,yd02;
      
      if (xc == 0) {
	xd01 = 0; yd01 = rc+yc;
	xd02 = 0; yd02 = -rc+yc; 
      } else {
	xd01 = xc+yc/xc*yc+sqrt(pow((xc+yc/xc*yc),2)-xc*xc-yc*yc+rc*rc); yd01 = yc/xc*xd01;
	xd02 = xc+yc/xc*yc-sqrt(pow((xc+yc/xc*yc),2)-xc*xc-yc*yc+rc*rc); yd02 = yc/xc*xd02;
      }
      
      double r1 = sqrt(xd01*xd01+yd01*yd01);
      double r2 = sqrt(xd02*xd02+yd02*yd02);
      
      double phiV;
      
      if (r1 < r2)
	phiV = atan2(yd01,xd01); 
      else
	phiV = atan2(yd02,xd02); 
      
      double phi1 = phiCorr(phiV+M_PI/2);
      double phi2 = phiCorr(phiV-M_PI/2);
      
      if (fabs(phiCorr(phi1-phi0))<=fabs(phiCorr(phi2-phi0)))
	newphi = phi1;
      else
	newphi = phi2;
      
      d0c = fabs(sqrt(xc*xc+yc*yc)-rc)*sin(phiV-newphi);
      
      double theta=2*atan2(exp(-eta),1);
      double theta_save=theta;
      theta = theta - (1+spt)/2*M_PI;
      if (theta>0) theta = theta_save;
      
      double deltaz0= -spt*rc/tan(theta)*phiCorr(phiCorr(newphi)-phiCorr(phi0));
      z0c = z0 + deltaz0;
      
    }
  }


 protected:
  
  double m_beamX, m_beamY, m_beamZ;
  std::vector<TIDA::Track*> m_tracks;
  
};


#endif  // TrigInDetAnalysis_Converter_H
