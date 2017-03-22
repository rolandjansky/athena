/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TaggerHelper.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TaggerHelper.h"

#ifndef VALIDATION_TOOL

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
//#include "Particle/TrackParticleContainer.h"



#else

#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigBjetHypo/TrigBjetDataHelper.h"
#include "Root/FexAlgo.h"

#endif




//** ----------------------------------------------------------------------------------------------------------------- **//

#ifndef VALIDATION_TOOL

TaggerHelper::TaggerHelper(MsgStream& log, unsigned int logLvl) :
  m_log(log),
  m_logLvl(logLvl)
{}

#else

TaggerHelper::TaggerHelper(MsgStream& log, unsigned int logLvl, bool negativeWeights) :
  m_log(log),
  m_logLvl(logLvl)
{if(negativeWeights) m_sign=-1; else m_sign=1;}

#endif

//** ----------------------------------------------------------------------------------------------------------------- **//


TaggerHelper::~TaggerHelper() {}


//** ----------------------------------------------------------------------------------------------------------------- **//

#ifndef VALIDATION_TOOL

void TaggerHelper::showParam(const xAOD::TrackParticle*& track, unsigned int i) {
  
  //* Print track impact parameters at EF *//
  m_log << MSG::VERBOSE << "getParam method" << endmsg;
  m_log << MSG::VERBOSE << " Track number " << i+1 << endmsg;
  m_log << MSG::VERBOSE 
	<< "  d0 = "    << track->d0() 
	<< "  z0 = "    << track->z0()
	<< "  theta = " << track->theta() 
	<< "  phi0 = "  << track->phi0() 
	<< "  pT = "    << track->pt() 
	<< endmsg;
  m_log << MSG::VERBOSE 
	<< "  errd0 = "    << track->definingParametersCovMatrix()( Trk::d0,   Trk::d0   )
    	<< "  errz0 = "    << track->definingParametersCovMatrix()( Trk::z0,   Trk::z0   )
	<< "  errphi0 = "  << track->definingParametersCovMatrix()( Trk::phi0, Trk::phi0 )
	<< "  errtheta = " << track->definingParametersCovMatrix()( Trk::theta,Trk::theta)
    //<< "  errpT = "    << track->pt() 
	<< endmsg;
  
}


//** ----------------------------------------------------------------------------------------------------------------- **//


unsigned int TaggerHelper::getTrackNumber(const xAOD::TrackParticleContainer* pointerToEFTrackCollections) {

  unsigned int nTracks;

  if (pointerToEFTrackCollections) {
    nTracks = pointerToEFTrackCollections->size();
    if (m_logLvl <= MSG::DEBUG)  
      m_log << MSG::DEBUG << "Found " << nTracks << " tracks in the RoI" << endmsg;
  } else {
    nTracks = 0;
    if (m_logLvl <= MSG::DEBUG)  
      m_log << MSG::DEBUG << "No tracks in the RoI" << endmsg;
  }

  return nTracks;
}

		      
//** ----------------------------------------------------------------------------------------------------------------- **//


unsigned int TaggerHelper::getVerticesNumber(const TrigVertexCollection*& pointerToPrmVtxCollections) {

  unsigned int nVertices;

  if (pointerToPrmVtxCollections) {
    nVertices = pointerToPrmVtxCollections->size();
    if (m_logLvl <= MSG::DEBUG)  
      m_log << MSG::DEBUG << "Found " << nVertices << " vertices in the RoI" << endmsg;
  } else {
    nVertices = 0;
    if (m_logLvl <= MSG::DEBUG)  
      m_log << MSG::DEBUG << "No vertices in the RoI" << endmsg;
  }

  return nVertices;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


unsigned int TaggerHelper::getVerticesNumber(const VxContainer*& pointerToPrmVtxCollections) {

  unsigned int nVertices;

  if (pointerToPrmVtxCollections) {
    nVertices = pointerToPrmVtxCollections->size();
    if (m_logLvl <= MSG::DEBUG)  
      m_log << MSG::DEBUG << "Found " << nVertices << " primary vertices in the RoI" << endmsg;
  } else {
    nVertices = 0;
    if (m_logLvl <= MSG::DEBUG)  
      m_log << MSG::DEBUG << "No vertices in the RoI" << endmsg;
  }

  return nVertices;
}

#else

unsigned int TaggerHelper::getTrackNumber(const std::vector<TrigBjetTrackInfo> &v) {
  return v.size();
}


#endif

//** ----------------------------------------------------------------------------------------------------------------- **//


float TaggerHelper::signedD0(float d0, float phi0, float jetPhi) {
 
  float sdca = TMath::Abs(d0);
  float deltaPhi = phiCorr(phiCorr(jetPhi) - phi0);
  float sign = d0*TMath::Sin(deltaPhi);
  if(sign < 0) sdca = -sdca;
#ifndef VALIDATION_TOOL
  return sdca;
#else
  return m_sign*sdca;
#endif
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TaggerHelper::signedZ0(float z0, float eta, float jetEta) {

  float sz0 = TMath::Abs(z0);
  float deltaEta = jetEta - eta;
  float sign = z0*deltaEta;
  if (sign < 0) sz0 = -sz0; 
#ifndef VALIDATION_TOOL
  return sz0;
#else
  return m_sign*sz0;
#endif

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
    
    if(xBeamSpot != 0 && yBeamSpot != 0 && m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "d0 (shifted) = " << d0c << " , d0 (referred to origin) = " << d0 << endmsg;
  } else {

    double rc = fabs(pt)*15.0/(9.0*1.042);
    
    double xc = (fabs(d0)-spt*sd0*rc)*cos(phi0+M_PI/2*sd0) - xBeamSpot;
    double yc = (fabs(d0)-spt*sd0*rc)*sin(phi0+M_PI/2*sd0) - yBeamSpot;
    
    if(xBeamSpot != 0 && yBeamSpot != 0 && m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "Coordinates of the circle center in transverse plane = (" << xc << "," << yc << ")" << endmsg;

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

    if(xBeamSpot != 0 && yBeamSpot != 0 && m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "z0 (shifted) = " << z0c << " , z0 (referred to origin) = " << z0 << endmsg;
    if(xBeamSpot != 0 && yBeamSpot != 0 && m_logLvl <= MSG::VERBOSE)
      m_log << MSG::VERBOSE << "d0 (shifted) = " << d0c << " , d0 (referred to origin) = " << d0 << endmsg;

  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//

#ifndef VALIDATION_TOOL

//double TaggerHelper::VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, const std::vector<double> SecVrtErr, float& Signif) {
double TaggerHelper::VrtVrtDist(const xAOD::Vertex& PrimVrt, const Amg::Vector3D & SecVrt, const std::vector<double> SecVrtErr, float& Signif) {

//const xAOD::Vertex* PrimVrt;	
/*  double distx =  PrimVrt.position().x()- SecVrt.x();
  double disty =  PrimVrt.position().y()- SecVrt.y();
  double distz =  PrimVrt.position().z()- SecVrt.z();
*/
  float distx =  PrimVrt.x()- SecVrt.x();
  float disty =  PrimVrt.y()- SecVrt.y();
  float distz =  PrimVrt.z()- SecVrt.z();

  AmgSymMatrix(3) PrimCovMtx;  //Create
  PrimCovMtx.setZero(); // Set to zero
  PrimCovMtx(0,0)                   = SecVrtErr[0]+PrimVrt.covariancePosition()(0,0);
  PrimCovMtx(0,1) = PrimCovMtx(1,0) = SecVrtErr[1]+PrimVrt.covariancePosition()(0,1);
  PrimCovMtx(1,1)                   = SecVrtErr[2]+PrimVrt.covariancePosition()(1,1);
  PrimCovMtx(0,2) = PrimCovMtx(2,0) = SecVrtErr[3]+PrimVrt.covariancePosition()(0,2);
  PrimCovMtx(1,2) = PrimCovMtx(2,1) = SecVrtErr[4]+PrimVrt.covariancePosition()(1,2);
  PrimCovMtx(2,2)                   = SecVrtErr[5]+PrimVrt.covariancePosition()(2,2);

  bool isInvertible;
  AmgSymMatrix(3) WgtMtx;
  WgtMtx.setZero();
  PrimCovMtx.computeInverseWithCheck(WgtMtx, isInvertible);
  if(isInvertible) {
    Signif = distx*WgtMtx(0,0)*distx
      +disty*WgtMtx(1,1)*disty
      +distz*WgtMtx(2,2)*distz
      +2.*distx*WgtMtx(0,1)*disty
      +2.*distx*WgtMtx(0,2)*distz
      +2.*disty*WgtMtx(1,2)*distz;
    if(Signif>=0)
      Signif=sqrt(Signif);
    else
      Signif = 0.;
  } else {
    Signif = 0.;
  }

  return sqrt(distx*distx+disty*disty+distz*distz);
}

#endif

//** ----------------------------------------------------------------------------------------------------------------- **//
  

