/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigInDetKarimakiFitter tool
// -------------------------------
// ATLAS Collaboration
//
// 13.02.2007 Package created
//
// Author: Carlo Schiavi, Genova
// e-mail: Carlo.Schiavi@ge.infn.it
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>

#include "StoreGate/StoreGateSvc.h" 

#include "GaudiKernel/ToolFactory.h"
#include "StoreGate/DataHandle.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetTrackFitter/TrigInDetKarimakiFitter.h"



/*********************************************************************************/
/** Constructor                                                                 **/
/*********************************************************************************/
TrigInDetKarimakiFitter::TrigInDetKarimakiFitter(const std::string& t, 
						 const std::string& n,
						 const IInterface* p ) : AthAlgTool(t,n,p) {
  
  // Declare interface
  declareInterface< ITrigInDetTrackFitter >( this );

  // Declare properties
  //declareProperty( "Chi2Cut", m_DChi2 = 1000.0);
}


/*********************************************************************************/
/** Destructor                                                                  **/
/*********************************************************************************/
TrigInDetKarimakiFitter::~TrigInDetKarimakiFitter() {
}


/*********************************************************************************/
/** initialize                                                                  **/
/*********************************************************************************/
StatusCode TrigInDetKarimakiFitter::initialize() {

  // Base init and get message svc
  m_log = new MsgStream(msgSvc(), name());
  StatusCode sc = AthAlgTool::initialize();
  if(sc.isFailure()) {
    (*m_log) << MSG::FATAL << "unable to initialize" << endreq;
    return StatusCode::FAILURE;
  }
 
  // Retrieve Detector Description
  StoreGateSvc* detectorStore;
  sc = service("DetectorStore", detectorStore);
  if(sc.isFailure()) {
    (*m_log) << MSG::FATAL << "unable to locate Detector Store" << endreq;
    return StatusCode::FAILURE;
  }

  // Get SCT & pixel Identifier helpers
  if (detectorStore->retrieve(m_pixelId, "PixelID").isFailure()) { 
     (*m_log) << MSG::FATAL << "Could not get Pixel ID helper" << endreq;
     return StatusCode::FAILURE; 
  }
                                                                                                                                           
  if (detectorStore->retrieve(m_sctId, "SCT_ID").isFailure()) {
     (*m_log) << MSG::FATAL << "Could not get SCT ID helper" << endreq;
     return StatusCode::FAILURE;
  }
  
  (*m_log) << MSG::INFO << "TrigInDetKarimakiFitter constructed" << endreq;
  return sc;
}


/*********************************************************************************/
/** finalize                                                                    **/
/*********************************************************************************/
StatusCode TrigInDetKarimakiFitter::finalize() {

  // Destroy message svc
  delete m_log;

  // Base finalize
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}


/*********************************************************************************/
/** fit                                                                         **/
/*********************************************************************************/
void TrigInDetKarimakiFitter::fit(TrigInDetTrackCollection* tracks) {

  // Loop on tracks
  TrigInDetTrackCollection::iterator t, tEnd=tracks->end();
  for(t=tracks->begin(); t!=tEnd; t++) {
    
    // Ancillary track
    TrigInDetKarimakiFitter::AncillaryTrack at;
    
    // Perform three point fit
    if((*t)->siSpacePoints()->size()>2) {
      circlefitRPhi(*t, at);
      fitRZ(*t, at);
    }

    // Perform full fit
    if((*t)->siSpacePoints()->size()>3) {
      fitRPhi(*t, at);
    }
    
    // Evaluate final chi2 value  
    (*t)->chi2(sqrt(at.chi2rz*at.chi2rz + at.chi2rp*at.chi2rp));
  }
}


/*********************************************************************************/
/** circlefitRPhi                                                               **/
/*********************************************************************************/
void TrigInDetKarimakiFitter::circlefitRPhi(TrigInDetTrack* t, TrigInDetKarimakiFitter::AncillaryTrack& at) {
  
  /** Fitting variables. **/
  double phi=0, d0=0;
  double x[3], y[3];
  double tr, cx1, cy1, cx2, cy2, xm1, ym1, xm2, ym2;
  double phi0, dph0p;

  /** Get space point indexes. **/
  double rsp, rval[3]={10000.0, 10000.0, 10000.0};
  int ind, rind[3]={0, 1, 2};

  std::vector<const TrigSiSpacePoint*>::iterator sp, spEnd=t->siSpacePoints()->end();
  for(sp=t->siSpacePoints()->begin(), ind=0; sp!=spEnd; sp++, ind++) {
    // Save indices for preliminary RPhi fit
    rsp = (*sp)->r();
    if(rsp<rval[0])                          {rval[0]=rsp; rind[0]=ind;}
    if(rsp>122.5 && (rsp-122.5)<rval[1])     {rval[1]=rsp-122.5; rind[1]=ind;}
    if(rsp>350.0 && (rsp-350.0)<rval[2])     {rval[2]=rsp-350.5; rind[2]=ind;}
  }
  if(rind[0]==rind[1] || rind[1]==rind[2]) {rind[0]=0; rind[1]=1; rind[2]=t->siSpacePoints()->size()-1;}

  /** Get space point coordinates. */
  for(int i=0; i<3; i++) {
    x[i] = (*(t->siSpacePoints()))[rind[i]]->x();
    y[i] = (*(t->siSpacePoints()))[rind[i]]->y();
  }

  /** Evaluate useful quantities. */
  xm1 = (x[1]+x[0])/2;
  ym1 = (y[1]+y[0])/2;
  cx1 = y[1] - y[0];
  cy1 = x[0] - x[1];
	
  xm2 = (x[1]+x[2])/2;
  ym2 = (y[1]+y[2])/2;
  cx2 = y[1] - y[2];
  cy2 = x[2] - x[1];
	
  /** Check if track is too straight. */
  bool straight=false;
  if(cy1/cx1 == cy2/cx2) {
    straight=true;
  }
  else {
    // Evaluate circle radius
    tr = (xm2-xm1+(ym1-ym2)*cx2/cy2)/(cx1-cy1*cx2/cy2);
    at.xc = xm1 + cx1*tr;
    at.yc = ym1 + cy1*tr;
    at.rc = sqrt((at.xc-x[0])*(at.xc-x[0])+(at.yc-y[0])*(at.yc-y[0]));
    at.pt = at.rc*1.042*9.0/15.0;
    if(at.pt>=1000000.0) straight=true;
  }

  /** Treat straight tracks. */
  if(straight) {
    phi = atan2(y[1]-y[0], x[1]-x[0]);
    if(phi<-M_PI) phi+=2*M_PI;
    if(phi>M_PI)  phi-=2*M_PI;
    at.pt = 1000000.0;
    at.xc = at.yc = at.rc = 0;
    double phiV = atan2(y[0], x[0]);
    d0 = sqrt(x[0]*x[0]+y[0]*y[0])*sin(phiV-phi);
  } 
  /** Treat curved tracks. */
  else {
    at.phic = atan2(at.xc, -at.yc);
    d0 = sqrt(at.xc*at.xc+at.yc*at.yc)-at.rc;
    /** Evaluate charge. */
    phi0 = atan2(at.xc-x[0],-at.yc+y[0]);
    dph0p = phi0-at.phic;
    if (dph0p >  M_PI) dph0p = dph0p - 2*M_PI;
    if (dph0p < -M_PI) dph0p = dph0p + 2*M_PI;
    if (dph0p > 0) {
      at.q = -1;
    } else {
      at.q = 1;
    }
    at.pt = at.pt*at.q;
      
    /** Evaluate phi0. */
    phi = atan2(-at.xc, at.yc) + (1+at.q)/2*M_PI;
    if(phi<-M_PI) phi+=2*M_PI;
    if(phi>M_PI)  phi-=2*M_PI;
    
    /** Evaluate d0 sign. */
    double absd0  = fabs(d0);
    double phiV = atan2(y[0], x[0]);
    if(phiV<-M_PI) phiV+=2*M_PI;
    if(phiV> M_PI) phiV-=2*M_PI;
    float newsign = sin(phiV-phi);
    if (newsign<0){
      d0 = -absd0;
    } else {
      d0 =  absd0;
    }    
  }
    
  /** Save track parameters. */
  TrigInDetTrackFitPar* par = const_cast<TrigInDetTrackFitPar*>(t->param()); 
  par->a0(d0);
  par->phi0(phi);
  par->pT(at.pt);

  /** Save chi square value . */
  at.nSP = 3;
  at.chi2rp = 0;
}



/*********************************************************************************/
/*********************************************************************************/
/** fitRZ                                                                       **/
/*********************************************************************************/
/*********************************************************************************/
void TrigInDetKarimakiFitter::fitRZ(TrigInDetTrack* t, TrigInDetKarimakiFitter::AncillaryTrack& at) {
	
  /** Fill sum variables. */

  // Sum variable
  double n=0, rr=0, rz=0, r=0, z=0;
  double iphi, ir, iz, ezez;

  // Preliminary track parameters
  TrigInDetTrackFitPar* par = const_cast<TrigInDetTrackFitPar*>(t->param()); 
  double z1=(*(t->siSpacePoints()))[0]->z(), z2=(*(t->siSpacePoints()))[1]->z();
  double r1=(*(t->siSpacePoints()))[0]->r(), r2=(*(t->siSpacePoints()))[1]->r();
  at.cotantheta = (z2-z1)/(r2-r1);

  // Loop on SPs
  std::vector<const TrigSiSpacePoint*>::iterator sp, spEnd=t->siSpacePoints()->end();
  for(sp=t->siSpacePoints()->begin(); sp!=spEnd; sp++) {
    // Evaluate error on Z
    ezez = ((*sp)->dz()*(*sp)->dz()) + (at.cotantheta*at.cotantheta)*((*sp)->dr()*(*sp)->dr());
    // Evaluate sum variables
    iphi = atan2(at.xc-(*sp)->x(), -at.yc+(*sp)->y());
    ir = iphi-at.phic; if(ir>M_PI) ir=ir-2*M_PI; if(ir<-M_PI) ir=ir+2*M_PI;
    iz = (*sp)->z();
    n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
  }


  /** Evaluate line parameters. */
  double invdelta, z0;
  invdelta = 1/(n*rr-r*r);
  z0       = invdelta*(rr*z-r*rz);
  at.ang = invdelta*(n*rz-r*z);


  /** Evaluate track parameters. */
  double theta = atan2(-at.q*at.rc, at.ang);
  if(theta<0) theta = theta + (1+at.q)/2*M_PI;
  par->eta(-log(tan(theta/2)));
  par->z0(z0);


  /** Evaluate chi square. */
  at.cotantheta = 1/tan(theta);
  double chi2 = 0;
  for(sp=t->siSpacePoints()->begin(); sp!=spEnd; sp++) {
    // Evaluate error on Z
    ezez = ((*sp)->dz()*(*sp)->dz()) + (at.cotantheta*at.cotantheta)*((*sp)->dr()*(*sp)->dr());
    // Evaluate sum variables
    iphi = atan2(at.xc-(*sp)->x(), -at.yc+(*sp)->y());
    ir = iphi-at.phic; if(ir>M_PI) ir=ir-2*M_PI; if(ir<-M_PI) ir=ir+2*M_PI;
    iz = (*sp)->z();
    double izgood = z0 + at.ang*ir;
    chi2+=(iz-izgood)*(iz-izgood)/ezez;
  }

  /* Save chi square value. */
  at.chi2rz = chi2;
  at.nSP = t->siSpacePoints()->size();
}



/*********************************************************************************/
/*********************************************************************************/
/** fitRPhi                                                                     **/
/*********************************************************************************/
/*********************************************************************************/
void TrigInDetKarimakiFitter::fitRPhi(TrigInDetTrack* t, TrigInDetKarimakiFitter::AncillaryTrack& at) {

  /** Fill sum variables. */
  // Sum variable
  double Cxx, Cxy, Cyy, Cxrr, Cyrr, Crrrr, q1, q2;
  double Sw=0, x=0, y=0, xx=0, xy=0, yy=0, xrr=0, yrr=0, rr=0, rrrr=0;
  double ix, iy, ir, iphi, iphic, ee;
  double phi, rho, d0;

  // Preliminary track parameters
  TrigInDetTrackFitPar* par = const_cast<TrigInDetTrackFitPar*>(t->param()); 
  phi = par->phi0();

  // Loop on SPs
  std::vector<const TrigSiSpacePoint*>::iterator sp, spEnd=t->siSpacePoints()->end();
  for(sp=t->siSpacePoints()->begin(); sp!=spEnd; sp++) {
    // Get coordinates
    ix = (*sp)->x();
    iy = (*sp)->y();
    ir = (*sp)->r();
    iphi = (*sp)->phi();
    iphic = atan2(iy-at.yc, ix-at.xc);
    // Evaluate error
    ee = (ir*ir*(*sp)->dphi()*(*sp)->dphi()*cos(M_PI/2+iphi-iphic)*cos(M_PI/2+iphi-iphic)) +
         ((*sp)->dr()*(*sp)->dr()*sin(M_PI/2+iphi-iphic)*sin(M_PI/2+iphi-iphic));
    // Evaluate sum variables
    Sw+=1/ee; x+=ix/ee; y+=iy/ee; xx+=(ix*ix)/ee; xy+=(ix*iy)/ee; yy+=(iy*iy)/ee; xrr+=(ix*ir*ir)/ee; yrr+=(iy*ir*ir)/ee; rr+=(ir*ir)/ee; rrrr+=(ir*ir*ir*ir)/ee;
  }
  // Divide by weight sum
  x/=Sw; y/=Sw; xx/=Sw; xy/=Sw; yy/=Sw; xrr/=Sw; yrr/=Sw; rr/=Sw; rrrr/=Sw;


  /** Evaluate circle parameters. */
  Cxx=xx-x*x; Cxy=xy-x*y; Cyy=yy-y*y; Cxrr=xrr-x*rr; Cyrr=yrr-y*rr; Crrrr=rrrr-rr*rr;
  q1=(Crrrr*Cxy)-(Cxrr*Cyrr); q2=Crrrr*(Cxx-Cyy)-(Cxrr*Cxrr)+(Cyrr*Cyrr);

  double signx = cos(phi)/fabs(cos(phi)); double signy = sin(phi)/fabs(sin(phi));

  phi = 0.5*atan2(signy*2*q1/q2, signx);
  if(phi>M_PI) phi-=2*M_PI; else if(phi<-M_PI) phi+=2*M_PI;

  if(signx<0 && signy>=0) {
    if(phi<0) phi=M_PI/2-phi;
    else phi=M_PI-phi;
  }
  else if(signx>=0 && signy<0) {
    if(phi<0) phi=3*M_PI/2-phi;
    else phi=-phi;
  }
  else if(signx>=0 && signy>=0) {
    if(phi<0) phi=M_PI/2+phi;
    else phi=phi;
  }
  else if(signx<0 && signy<0) {
    if(phi<0) phi=3*M_PI/2+phi;
    else phi=M_PI+phi;
  }
  if(phi>M_PI) phi-=2*M_PI; else if(phi<-M_PI) phi+=2*M_PI;

  at.k     = (sin(phi)*Cxrr-cos(phi)*Cyrr)/Crrrr; 
  at.delta = -at.k*rr + sin(phi)*x - cos(phi)*y;
  rho        = 2*at.k/sqrt(1-4*at.delta*at.k);
  d0         = -2*at.delta/(1+sqrt(1-4*at.delta*at.k));

  /** Evaluate track parameters. */
  par->phi0(phi);
  par->a0(d0);
  par->pT(1.042*(1/rho)*9.0/15.0);


  /** Evaluate chi square. */
  double chi2 = Sw*(1+rho*d0)*(1+rho*d0)*(sin(phi)*sin(phi)*Cxx - 2*sin(phi)*cos(phi)*Cxy + cos(phi)*cos(phi)*Cyy - at.k*at.k*Crrrr);

  /* Save chi square value. */
  at.chi2rp = chi2;
  at.nSP = t->siSpacePoints()->size();
}	

