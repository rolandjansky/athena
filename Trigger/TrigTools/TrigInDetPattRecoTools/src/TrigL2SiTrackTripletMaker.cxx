/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigInDetPattRecoEvent/TrigL2SpacePointStorage.h"
#include "TrigInDetPattRecoEvent/TrigL2TimeoutException.h"
#include "TrigInDetPattRecoTools/TrigL2SiTrackTripletCuts.h"
#include "TrigInDetPattRecoTools/TrigL2SiTrackTripletMaker.h"

#include "AthenaKernel/Timeout.h"

#include<algorithm>

//#define SITRACK_DEBUG 1

bool TrigL2SiTrackTripletMaker::operator()(TrigL2TrackCandidate& p) throw(TrigL2TimeoutException) {

  const float ptConst = 1.042*9.0/15.0;

  if(tripletMakingCount>=m_cuts.m_tripletMakingLimit) {
    return true;
  }

  // Check if event timeout was reached
  if (Athena::Timeout::instance().reached()) {
    throw TrigL2TimeoutException(2);// 2 - TripletMaker
  }

  if(p.deleted) 
    return false; 

  p.triplet_clear(); // all-in-one method

  float x1,x2,y1,y2,z1,z2,r1,r2,xm1,ym1,cx1,cy1;
  float phi1;
  float x12,y12,z12,r12;
  float r12inv;//,z12inv;
  float costheta_squared, r12_squared;
  float x12_scaled,y12_scaled;
  //z12_scaled,r12_scaled;
  float cotantheta;
  float ezezArray[2];

  //preliminary calculations ...
  const TrigSiSpacePointBase* s1 = p.seed_sp1();
  const TrigSiSpacePointBase* s2 = p.seed_sp2();
  x1 = s1->x();x2 = s2->x();
  y1 = s1->y();y2 = s2->y();
  z1 = s1->z();z2 = s2->z();
  r1 = s1->r();r2 = s2->r();
  phi1 = s1->phi();
  x12 = x2 - x1;
  y12 = y2 - y1;
  z12 = z2 - z1;
  r12 = r2 - r1;
  
  // Protect against division by zero
  if(fabs(r12)<0.00001) {
    r12 = (r12<0)?-0.00001 : 0.00001;
  }
  // Protect against division by zero
  if(fabs(z12)<0.00001) {
    z12 = (z12<0)?-0.00001 : 0.00001;
  }
  // extrapolation scale along R or Z
  r12inv = 1.0 / r12;
  //z12inv = 1.0 / z12;
  
  costheta_squared = r12*r12/(z12*z12+r12*r12);
  r12_squared = r12*r12;
  x12_scaled = x12*r12inv;
  y12_scaled = y12*r12inv;
  //  z12_scaled = z12*r12inv;
  //r12_scaled = r12*z12inv;
  // rphi fit variables
  xm1 = (x2+x1)*0.5;
  ym1 = (y2+y1)*0.5;
  cx1 = y12; 
  if(cx1==0) 
    cx1=0.0001;
  cy1 = -x12; 
  if(cy1==0) 
    cy1=0.0001;
  
  // rz fit variables
  cotantheta = z12/r12;
  float cotantheta2 = cotantheta*cotantheta;
  ezezArray[0] = (s1->dz()*s1->dz())+cotantheta2*(s1->dr()*s1->dr());
  ezezArray[1] = (s2->dz()*s2->dz())+cotantheta2*(s2->dr()*s2->dr());

  for(unsigned int lay = p.seed_sp2()->layer()+1; lay<MAX_SILICON_LAYER_NUM; lay++) {

    // Loop on space points of each layer

    const std::vector<const TrigSiSpacePointBase*>& vsp = m_spLay[lay];

    float cutOnRz = cut_rz[lay]*r12_squared;

    for(std::vector<const TrigSiSpacePointBase*>::const_iterator it = vsp.begin(); it!=vsp.end(); ++it) {

      // Check layer matching: barrel
      //if(fabs((z1 + z12_scaled*(m_geoR[lay]-r1)))<m_geoZmin[lay] || fabs((z1 + z12_scaled*(m_geoR[lay]-r1)))>m_geoZmax[lay]) continue;
      // Check layer matching: endcap
      //if(fabs((r1 + r12_scaled*(m_geoZ[lay]-z1)))<m_geoRmin[lay] || fabs((r1 + r12_scaled*(m_geoZ[lay]-z1)))>m_geoRmax[lay]) 
      //{rejLayMatch++; continue;}  
      
      // Cut on rz
            
      const TrigSiSpacePointBase* sp3 = (*it);

      float r3 = sp3->r();
      float dr31 = r3-r1;
      
      float value = (z1-sp3->z())*r12+dr31*z12;
      value = value*value*costheta_squared;
      
      if(value > cutOnRz) continue;

      // Cut on rphi

      float x3 = sp3->x();
      float y3 = sp3->y();
      

      float deltaX = x1 + x12_scaled*dr31 - x3;
      float deltaY = y1 + y12_scaled*dr31 - y3;

      value = deltaX*deltaX + deltaY*deltaY; 
      
      float cut = cut_rphi*(r3-r2)*(r3-r2)*(r3-r2);
      
      if(value > cut) continue;
      
      // Fit in rphi
      float xm2 = (x2+x3)*0.5;
      float ym2 = (y2+y3)*0.5;
      float cx2 = (y2-y3); 
      if(cx2==0) {
	cx2=0.0001;
      }
      float cy2 = (x3-x2); 
      if(cy2==0) {
	cy2=0.0001;
      }
      
      // Cut on too straight tracks
      if(cy2==0) continue;
      
      float tr = (cx1-cy1*cx2/cy2); 
      if(tr==0) continue;
      
      // Evaluate circle radius
      tr = (xm2-xm1+(ym1-ym2)*cx2/cy2)/tr;
      float xc = xm1 + cx1*tr;
      float yc = ym1 + cy1*tr;
      float rc = sqrt((xc-x1)*(xc-x1)+(yc-y1)*(yc-y1));
      // Cut on d0
      float d0 = fabs(sqrt(xc*xc+yc*yc)-rc);
            
      if(d0>cut_d0) {
	continue;
      }
      // Cut on pT
      float pt = rc*ptConst;
      if(pt<cut_pt) { 
	continue;
      }
      // Evaluate phi
      float phi  = atan2(-xc, yc);
      float dphi = phi-phi1;
      
      if(dphi<-M_PI) 
	dphi+=2*M_PI;
      if(dphi>M_PI)  
	dphi-=2*M_PI;
      if(dphi<-M_PI*0.5) 
	phi+=M_PI;
      if(dphi>M_PI*0.5)  
	phi-=M_PI;
      if(phi<-M_PI) 
	phi+=2*M_PI;
      if(phi>M_PI)  
	phi-=2*M_PI;

      float diff_phi = phi - m_cuts.m_roiPhi;
      if(diff_phi<-M_PI) 
	diff_phi+=2*M_PI;
      if(diff_phi>M_PI)  
	diff_phi-=2*M_PI;
      if(fabs(diff_phi)>cut_phi) {
	continue;
      }

      // Fit in rz
      float phic = atan2(xc, -yc);
      float n=0,rr=0,rz=0,r=0,z=0;
      
      // sp1
      float ezez = ezezArray[0];
      float iphi = atan2(xc-x1, -yc+y1);
      float ir = iphi-phic; 
      if(ir>M_PI) 
	ir=ir-2*M_PI; 
      if(ir<-M_PI) 
	ir=ir+2*M_PI;
      float iz = z1;
      n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
      
      // sp2
      ezez = ezezArray[1];
      
      iphi = atan2(xc-x2, -yc+y2);
      ir = iphi-phic; 
      if(ir>M_PI) 
	ir=ir-2*M_PI; 
      if(ir<-M_PI) 
	ir=ir+2*M_PI;
      iz = z2;
      n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
      
      // sp3
      ezez =(sp3->dz()*sp3->dz())+cotantheta2*(sp3->dr()*sp3->dr());
      iphi = atan2(xc-x3, -yc+y3);
      ir = iphi-phic; 
      if(ir>M_PI) 
	ir=ir-2*M_PI; 
      if(ir<-M_PI) 
	ir=ir+2*M_PI;
      iz = sp3->z();
      n+=1/ezez; rr+=(ir*ir)/ezez; rz+=(ir*iz)/ezez; r+=ir/ezez; z+=iz/ezez;
	
      // Evaluate eta
      if((n*rr-r*r)==0) continue;
      
      float ang = (n*rz-r*z)/(n*rr-r*r);

      float theta = atan2(rc, ang); 
      if(theta<-M_PI) 
	theta+=2*M_PI; 
      if(theta>M_PI) 
	theta-=2*M_PI; 
      if(theta<0) 
	theta=M_PI+theta;
      if((theta>(M_PI*0.5) && z12>0) || (theta<(M_PI*0.5) && z12<0))
	theta=M_PI-theta;
      
      float eta = -log(tan(theta*0.5));
  
      // Cut on eta-phi
      float diff_eta = eta - m_cuts.m_roiEta;
      
      if(fabs(diff_eta)>cut_eta) { 
	continue;
      }
     
      // Save triplet
      if(p.triplet_num<100) { 
#ifdef SITRACK_DEBUG
	std::cout<<"["<<sp3->index()<<"] "<<"d0="<<d0<<" phi="<<phi<<" eta="<<eta<<" pt="<<pt<<std::endl;
#endif
	p.addThirdSpacePoint(sp3,d0,p.m_params.m_z0,phi,eta,pt,1e8);// all-in-one method, big initial chi2
      }
      tripletMakingCount++;
      if(tripletMakingCount>=m_cuts.m_tripletMakingLimit) {
	break;
      }
    }
    if(tripletMakingCount>=m_cuts.m_tripletMakingLimit) {
      break;
    }
  }

  bool retVal = (tripletMakingCount>=m_cuts.m_tripletMakingLimit);
  
  //some post-processing

  // Delete trk if too few triplets are found

  if((int)(p.triplet_num)<(m_cuts.m_dynamic_extensionCutSpacePoints-2)) {
    p.deleted = true;
#ifdef SITRACK_DEBUG
    std::cout<<"DBG SEED: "<<"["<<p.seed_sp1()->index()<<"]-["<<p.seed_sp2()->index()<<"] REJECTED"<<std::endl;
#endif
    return retVal;
  }
  // Increase counter
  m_trks_num_active++;

  // Evaluate chi2 of triplets
  
  float phi12 = atan2(y12, x12);
  float eta12;
  if(phi12>M_PI) 
    phi12=phi12-2*M_PI; 
  if(phi12<-M_PI) 
    phi12=phi12+2*M_PI;
  float theta12 = atan2(r12,z12);
  if(theta12<-M_PI) 
    theta12+=2*M_PI; 
  if(theta12>M_PI) 
    theta12-=2*M_PI; 
  if(theta12<0) 
    theta12=M_PI+theta12;
 
 float tanHalfTheta=tan(0.5*theta12);
 if(tanHalfTheta!=0)
   eta12 = -log(tanHalfTheta);
 else {
   if(z12>0) 
     eta12=4.0;
   else 
     eta12=-4.0;
 }
  
#ifdef SITRACK_DEBUG
    std::cout<<"DBG SEED: "<<"["<<p.seed_sp1()->index()<<"]-["<<p.seed_sp2()->index()<<"] ACCEPTED"<<std::endl;
#endif
    
 for(unsigned int trip=0; trip<p.triplet_num; trip++) {
   TrigL2TrackParameters& param = p.tripletParameters(trip);
   float diff_eta = param.m_eta-eta12;
   float diff_phi = param.m_phi0-phi12;
   if(diff_phi<-M_PI) 
     diff_phi+=2*M_PI;
   if(diff_phi>M_PI)  
     diff_phi-=2*M_PI;
   param.m_chi2 = diff_eta*diff_eta+diff_phi*diff_phi;
 }
   
 return retVal;
}
