/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************
//
// NAME:     TaggerHelper.cxx
// PACKAGE:  Trigger/TrigMonitoring/TrigBjetMonitoring
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// *****************************************************

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrigBjetMonitoring/TaggerHelper.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "Particle/TrackParticleContainer.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TaggerHelper::TaggerHelper(MsgStream* log) :
  m_log(log)
{}


//** ----------------------------------------------------------------------------------------------------------------- **//


TaggerHelper::~TaggerHelper() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TaggerHelper::showParam(const TrigInDetTrack*& track, unsigned int i) {

  //* Print track impact parameters at L2 *//
  *m_log << MSG::VERBOSE << "Print track impact parameters at L2:" << endreq;
  *m_log << MSG::VERBOSE << " Track number " << i+1 << endreq;
  *m_log << MSG::VERBOSE << "  d0 = " << track->param()->a0() 
	<< "  z0 = " << track->param()->z0() 
	<< "  eta = " << track->param()->eta() 
	<< "  phi0 = " << track->param()->phi0()
	<< "  pT = " << track->param()->pT() << endreq;
  *m_log << MSG::VERBOSE << "  errd0 = " << track->param()->ea0() 
	<< "  errz0 = " << track->param()->ez0() 
	<< "  errphi0 = " << track->param()->ephi0()
	<< "  erreta = " << track->param()->eeta() 
	<< "  errpT = " << track->param()->epT() << endreq;  

}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TaggerHelper::showParam(const Rec::TrackParticle*& track, unsigned int i) {
  
  //* Print track impact parameters at EF *//
  *m_log << MSG::VERBOSE << "getParam method" << endreq;
  *m_log << MSG::VERBOSE << " Track number " << i+1 << endreq;
  *m_log << MSG::VERBOSE << "  d0 = " << track->measuredPerigee()->parameters()[Trk::d0]  
	<< "  z0 = " << track->measuredPerigee()->parameters()[Trk::z0]
	<< "  eta = " << track->measuredPerigee()->parameters()[Trk::theta] 
	<< "  phi0 = " << track->measuredPerigee()->parameters()[Trk::phi0] 
	<< "  pT = " << track->pt() << endreq;
  *m_log << MSG::VERBOSE << "  errd0 = " << Amg::error(*(track->measuredPerigee()->covariance()),Trk::d0)
    	<< "  errz0 = " << Amg::error(*(track->measuredPerigee()->covariance()),Trk::z0)
	<< "  errphi0 = " << Amg::error(*(track->measuredPerigee()->covariance()),Trk::phi0)
	<< "  erreta = " << Amg::error(*(track->measuredPerigee()->covariance()),Trk::theta)
	<< "  errpT = " << track->pt() << endreq;
  
}


//** ----------------------------------------------------------------------------------------------------------------- **//


unsigned int TaggerHelper::getTrackNumber(const TrigInDetTrackCollection* pointerToL2TrackCollections) {

  unsigned int nTracks;

  if (pointerToL2TrackCollections) {
    nTracks = pointerToL2TrackCollections->size();
    *m_log << MSG::DEBUG << "Found " << nTracks << " tracks in the RoI" << endreq;
  } else {
    nTracks = 0;
    *m_log << MSG::DEBUG << "No tracks in the RoI" << endreq;
  }
  
  return nTracks;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


unsigned int TaggerHelper::getTrackNumber(const Rec::TrackParticleContainer* pointerToEFTrackCollections) {

  unsigned int nTracks;

  if (pointerToEFTrackCollections) {
    nTracks = pointerToEFTrackCollections->size();
    *m_log << MSG::DEBUG << "Found " << nTracks << " tracks in the RoI" << endreq;
  } else {
    nTracks = 0;
    *m_log << MSG::DEBUG << "No tracks in the RoI" << endreq;
  }

  return nTracks;
}

		      
//** ----------------------------------------------------------------------------------------------------------------- **//


unsigned int TaggerHelper::getVerticesNumber(const TrigVertexCollection*& pointerToPrmVtxCollections) {

  unsigned int nVertices;

  if (pointerToPrmVtxCollections) {
    nVertices = pointerToPrmVtxCollections->size();
    *m_log << MSG::DEBUG << "Found " << nVertices << " vertices in the RoI" << endreq;
  } else {
    nVertices = 0;
    *m_log << MSG::DEBUG << "No vertices in the RoI" << endreq;
  }

  return nVertices;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


unsigned int TaggerHelper::getVerticesNumber(const VxContainer*& pointerToPrmVtxCollections) {

  unsigned int nVertices;

  if (pointerToPrmVtxCollections) {
    nVertices = pointerToPrmVtxCollections->size();
    *m_log << MSG::DEBUG << "Found " << nVertices << " primary vertices in the RoI" << endreq;
  } else {
    nVertices = 0;
    *m_log << MSG::DEBUG << "No vertices in the RoI" << endreq;
  }

  return nVertices;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TaggerHelper::signedD0(float& d0, float phi0, float jetPhi) {
 
  float sdca = TMath::Abs(d0);
  float deltaPhi = phiCorr(phiCorr(jetPhi) - phi0);
  float sign = d0*TMath::Sin(deltaPhi);
  if(sign < 0) sdca = -sdca;
  return sdca;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TaggerHelper::signedZ0(float& z0, float eta, float jetEta) {

  float sz0 = TMath::Abs(z0);
  float deltaEta = jetEta - eta;
  float sign = z0*deltaEta;
  if (sign < 0) sz0 = -sz0;
  return sz0;

}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TaggerHelper::phiCorr(float phi){

  if (phi < -TMath::Pi()) phi += 2*TMath::Pi();
  if (phi >  TMath::Pi()) phi -= 2*TMath::Pi();

  return phi;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TaggerHelper::IPCorr(float d0, float z0, float& d0c, float& z0c, float phi0, float eta, float pt, float xBeamSpot, float yBeamSpot) {


  double sn  = sin(phi0);
  double cs  = cos(phi0);
  double sd0 = (d0 != 0 ? d0/fabs(d0) : 1);
  double spt = (pt != 0 ? pt/fabs(pt) : 1);
    
  if (fabs(pt) >= 1*CLHEP::TeV) {
    d0c = d0 + xBeamSpot*sn - yBeamSpot*cs;
    
    if(xBeamSpot != 0 && yBeamSpot != 0)
      *m_log << MSG::DEBUG << "d0 (shifted) = " << d0c << " , d0 (referred to origin) = " << d0 << endreq;
  } else {

    double rc = fabs(pt)*15.0/(9.0*1.042);
    
    double xc = (fabs(d0)-spt*sd0*rc)*cos(phi0+M_PI/2*sd0) - xBeamSpot;
    double yc = (fabs(d0)-spt*sd0*rc)*sin(phi0+M_PI/2*sd0) - yBeamSpot;

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

    if(xBeamSpot != 0 && yBeamSpot != 0) {
      *m_log << MSG::DEBUG << "z0 (shifted) = " << z0c << " , z0 (referred to origin) = " << z0 << endreq;
      *m_log << MSG::DEBUG << "d0 (shifted) = " << d0c << " , d0 (referred to origin) = " << d0 << endreq;
    }
  }
}
