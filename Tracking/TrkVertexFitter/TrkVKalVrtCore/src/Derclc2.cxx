/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include "TrkVKalVrtCore/Derivt.h"
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include <algorithm>

namespace Trk {

//
//   Pointing constraint calculation in new data model.
//
//      cnstV and cnstP values are used!!!
//-----------------------------------------------
extern void cfnewp(long int*, double*, double*, double*, double*, double*);
extern std::array<double, 4> getCnstParticleMom( VKTrack *, double );
extern vkalMagFld      myMagFld;

void  calcPointConstraint( VKPointConstraint * cnst )
{
    VKConstraintBase * base_cnst = (VKConstraintBase*) cnst;
    VKVertex * vk=cnst->getOriginVertex();
//
//  Magnetic field in fitted vertex position
    double cnstPos[3];
    cnstPos[0]=vk->refIterV[0]+vk->cnstV[0];
    cnstPos[1]=vk->refIterV[1]+vk->cnstV[1];
    cnstPos[2]=vk->refIterV[2]+vk->cnstV[2];
    double localField = myMagFld.getMagFld(cnstPos,(vk->vk_fitterControl).get());
    double magConst = localField * vkalMagCnvCst;
//
//
    int it,Charge=0;
    double ptot[4]={0.,0.,0.,0.};  
    int NTRK = vk->TrackList.size();
    VKTrack * trk;
    std::vector< std::array<double, 4> > pp(NTRK);
    for( it=0; it<NTRK; it++){
      trk = vk->TrackList[it];
      pp[it]=getCnstParticleMom( trk, localField );
      ptot[0] += pp[it][0];    
      ptot[1] += pp[it][1];    
      ptot[2] += pp[it][2];    
      ptot[3] += pp[it][3];    
      Charge += trk->Charge;
    }
//Summary particle at fitted vertex
    double Pt= sqrt(ptot[0]*ptot[0] + ptot[1]*ptot[1]) ;
    double mom= sqrt(ptot[2]*ptot[2] + Pt*Pt) ;
    double phi=atan2(ptot[1],ptot[0]);
    double theta=acos( ptot[2]/mom );
    if(theta<1.e-5) theta=1.e-5;
    if(theta>M_PI-1.e-5) theta=M_PI-1.e-5;
    double invR = magConst / Pt; if(Charge) invR *= Charge;   
//
// Fitted vertex position in global reference frame
    double curV[3] = {vk->refIterV[0]+vk->cnstV[0], vk->refIterV[1]+vk->cnstV[1],vk->refIterV[2]+vk->cnstV[2]};
    double Px=ptot[0];
    double Py=ptot[1];
    double Pz=ptot[2];
    double sinp = sin(phi);
    double cosp = cos(phi);
    double cott = 1./tan(theta);
//
//Derivatives
    double dPhiDpx = -Py/Pt/Pt;               //dPhi/dPx
    double dPhiDpy =  Px/Pt/Pt;               //dPhi/dPy
    //double dPhiDpz =  0;                    //dPhi/dPz
    double dThetaDpx =  Px*Pz/(Pt*mom*mom);   //dTheta/dPx
    double dThetaDpy =  Py*Pz/(Pt*mom*mom);   //dTheta/dPy
    double dThetaDpz = -Pt/(mom*mom);         //dTheta/dPz
    double dRDpx =  Px/(Pt*magConst);         //dR/dPx
    double dRDpy =  Py/(Pt*magConst);         //dR/dPy
    //double dRDpz =  0;                      //dR/dPz
    if(Charge){dRDpx /=Charge; dRDpy /=Charge;}

// Main part

    double a0,t,z0;
    double dA0dPhi,dZdPhi,dZdTheta,dA0dR;
    if( Charge==0 ) {
      a0 =-sinp * (cnst->getTargetVertex()[0] - curV[0]) +     //Perigee with respect to targetVertex
           cosp * (cnst->getTargetVertex()[1] - curV[1]);
      t  = cosp * (cnst->getTargetVertex()[0] - curV[0]) +
           sinp * (cnst->getTargetVertex()[1] - curV[1]);
      z0 = curV[2] + t*cott - cnst->getTargetVertex()[2];
//----------------------------------------------------------------------
// First constraint  a0=0
//
      dA0dPhi = -t;
      for( it=0; it<NTRK; it++){
         double invRi = vk->TrackList[it]->cnstP[2];
         base_cnst->f0t[it][0].X = 0.;      // dPx/dTheta==0, dPy/dTheta==0
         base_cnst->f0t[it][0].Y = dA0dPhi * (dPhiDpx*(-pp[it][1]) + dPhiDpy*( pp[it][0])) ;
         base_cnst->f0t[it][0].Z = dA0dPhi * (dPhiDpx*(-pp[it][0]) + dPhiDpy*(-pp[it][1]))/invRi ;
      }
      base_cnst->h0t[0].X =  sinp;     // dA0/dFitVertex==dA0/dCurV
      base_cnst->h0t[0].Y = -cosp;
      base_cnst->h0t[0].Z =  0.;
//----------------------------------------------------------------------
// Second constraint  z=0
//
      dZdPhi   = a0*cott;
      dZdTheta = -t/(sin(theta)*sin(theta));
      for( it=0; it<NTRK; it++){
         double invRi = vk->TrackList[it]->cnstP[2];
         double p2i = pp[it][0]*pp[it][0]+pp[it][1]*pp[it][1]+pp[it][2]*pp[it][2];
         double pti = sqrt(pp[it][0]*pp[it][0]+pp[it][1]*pp[it][1]); 
         base_cnst->f0t[it][1].X  = 0.;      // dPx/dTheta==0, dPy/dTheta==0                        // ../dTheta_i
         base_cnst->f0t[it][1].Y  = dZdPhi * (dPhiDpx*(-pp[it][1]) + dPhiDpy*( pp[it][0])) ;        // ../dPhi_i
         base_cnst->f0t[it][1].Z  = dZdPhi * (dPhiDpx*(-pp[it][0]) + dPhiDpy*(-pp[it][1]))/invRi;   // ../dInvR_i
         base_cnst->f0t[it][1].X += dZdTheta * (                                                  dThetaDpz*(-p2i/pti));        // ../dTheta_i
         base_cnst->f0t[it][1].Y += dZdTheta * (dThetaDpx*(-pp[it][1]) + dThetaDpy*( pp[it][0]) +  0.);                         // ../dPhi_;    dPz/dPhi_i=0
         base_cnst->f0t[it][1].Z += dZdTheta * (dThetaDpx*(-pp[it][0]) + dThetaDpy*(-pp[it][1]) + dThetaDpz*(-pp[it][2]))/invRi;// ../dInvR_i 
      }
      base_cnst->h0t[1].X = -cosp*cott;     // dZ/dCurV
      base_cnst->h0t[1].Y = -sinp*cott;
      base_cnst->h0t[1].Z =  1.;
//std::cout<<" test0="<<Charge<<", "<<a0<<", "<<z0<<", "<<t<<", "<<Px<<", "<<Py<<", "<<Pz<<'\n';
//
// The same for charged particle
//

    }else{ 
      int Sign = 1; if(Charge<0)Sign=-1;
      double R = 1./invR;
      double xc = -R*sinp + curV[0]; // centre of circle
      double yc =  R*cosp + curV[1];
      double diffx = cnst->getTargetVertex()[0] - xc; 
      double diffy = cnst->getTargetVertex()[1] - yc;
      double diff = sqrt(  diffx*diffx + diffy*diffy );
      double sindphi = (curV[0]-xc)*diffy - (curV[1]-yc)*diffx;
             sindphi =  sindphi/fabs(R)/diff;
      double dphi = asin( sindphi );
      double cosdphi=sqrt(fabs(1.-sindphi*sindphi)); if(cosdphi<1.e-10)cosdphi=1.e-10;
      a0 = R*(1.-diff/fabs(R));
      t  = dphi*R;
      z0 = curV[2] + t*cott - cnst->getTargetVertex()[2];
      double dTdXcur = fabs(R)/cosdphi * (cosp/diff-diffx*sindphi/(diff*diff)) * (-1.);
      double dTdYcur = fabs(R)/cosdphi * (sinp/diff-diffy*sindphi/(diff*diff)) * (-1.);
//----------------------------------------------------------------------
// First constraint  a0=0
//
      dA0dPhi =    - (diffx*cosp + diffy*sinp)/diff*fabs(R);
      dA0dR   = 1. - (diffx*sinp - diffy*cosp)/diff*R/fabs(R);
      for( it=0; it<NTRK; it++){
         double invRi = vk->TrackList[it]->cnstP[2];
         base_cnst->f0t[it][0].X  = 0.;      // dPx/dTheta==0, dPy/dTheta==0
         base_cnst->f0t[it][0].Y  = dA0dPhi * (dPhiDpx*(-pp[it][1]) + dPhiDpy*( pp[it][0])) ;
         base_cnst->f0t[it][0].Z  = dA0dPhi * (dPhiDpx*(-pp[it][0]) + dPhiDpy*(-pp[it][1]))/invRi ;
         base_cnst->f0t[it][0].X += 0.;     
         base_cnst->f0t[it][0].Y += dA0dR * (dRDpx*(-pp[it][1]) + dRDpy*( pp[it][0]));
         base_cnst->f0t[it][0].Z += dA0dR * (dRDpx*(-pp[it][0]) + dRDpy*(-pp[it][1]))/invRi ;
      }
      base_cnst->h0t[0].X =  Sign*diffx/diff;     // dA0/dFitVertex==dA0/dCurV
      base_cnst->h0t[0].Y =  Sign*diffy/diff;
      base_cnst->h0t[0].Z =  0.;
//----------------------------------------------------------------------
// Second constraint  z=0
//
      dZdPhi   = -a0*cott;
      dZdTheta = -t/sin(theta)/sin(theta);
      for( it=0; it<NTRK; it++){
         double invRi = vk->TrackList[it]->cnstP[2];
         double p2i = pp[it][0]*pp[it][0]+pp[it][1]*pp[it][1]+pp[it][2]*pp[it][2];
         double pti = sqrt(pp[it][0]*pp[it][0]+pp[it][1]*pp[it][1]); 
         base_cnst->f0t[it][1].X  = 0.;      // dPx/dTheta==0, dPy/dTheta==0                        // ../dTheta_i
         base_cnst->f0t[it][1].Y  = dZdPhi * (dPhiDpx*(-pp[it][1]) + dPhiDpy*( pp[it][0])) ;        // ../dPhi_i
         base_cnst->f0t[it][1].Z  = dZdPhi * (dPhiDpx*(-pp[it][0]) + dPhiDpy*(-pp[it][1]))/invRi;   // ../dInvR_i
         base_cnst->f0t[it][1].X += dZdTheta * (                                                  dThetaDpz*(-p2i/pti));        // ../dTheta_i
         base_cnst->f0t[it][1].Y += dZdTheta * (dThetaDpx*(-pp[it][1]) + dThetaDpy*( pp[it][0]) +  0.);                         // ../dPhi_;    dPz/dPhi_i=0
         base_cnst->f0t[it][1].Z += dZdTheta * (dThetaDpx*(-pp[it][0]) + dThetaDpy*(-pp[it][1]) + dThetaDpz*(-pp[it][2]))/invRi;// ../dInvR_i 
      }
      base_cnst->h0t[1].X =  dTdXcur*cott;     // dZ/dCurV
      base_cnst->h0t[1].Y =  dTdYcur*cott;
      base_cnst->h0t[1].Z =  1.;
//std::cout<<" test1="<<Charge<<", "<<a0<<", "<<z0<<"  dst="<<t<<" NTRK="<<NTRK<<'\n';
    }
    base_cnst->aa[0]=a0;
    base_cnst->aa[1]=z0;


// -----------------------Check of propagation
//   double paro[5],parn[5],s,ref[3],pere[3]; long int ichg=0;
//   paro[0]=0.; paro[1]=0.; paro[2]=theta; paro[3]=phi; paro[4]=invR;
//   ref[0]=cnst->getTargetVertex()[0]-curV[0]; ref[1]=cnst->getTargetVertex()[1]-curV[1]; ref[2]=cnst->getTargetVertex()[2]-curV[2]; 
//   cfnewp(&ichg, paro, ref, &s, parn, pere);
//   std::cout<<" testp="<<Charge<<", "<<parn[0]<<", "<<parn[1]<<", "<<s<<'\n';
// -----------------------Derivatives
//std::cout<<base_cnst->f0t[0][0].X<<", "<<base_cnst->f0t[0][0].Y<<", "<<base_cnst->f0t[0][0].Z<<'\n';
//std::cout<<base_cnst->f0t[0][1].X<<", "<<base_cnst->f0t[0][1].Y<<", "<<base_cnst->f0t[0][1].Z<<'\n';
//std::cout<<base_cnst->f0t[1][0].X<<", "<<base_cnst->f0t[1][0].Y<<", "<<base_cnst->f0t[1][0].Z<<'\n';
//std::cout<<base_cnst->f0t[1][1].X<<", "<<base_cnst->f0t[1][1].Y<<", "<<base_cnst->f0t[1][1].Z<<'\n';
//std::cout<<base_cnst->h0t[0].X<<", "<<base_cnst->h0t[0].Y<<", "<<base_cnst->h0t[0].Z<<'\n';
//std::cout<<base_cnst->h0t[1].X<<", "<<base_cnst->h0t[1].Y<<", "<<base_cnst->h0t[1].Z<<'\n';
//std::cout<<"-------"<<'\n';   
//--------------------------------------------------------------------------------------------------
   if( cnst->onlyZ() ) {      //Z only constraint  <= all Rphi ralated stuff is 0
      for( it=0; it<NTRK; it++){
         base_cnst->f0t[it][0].X  = 0.001;  
         base_cnst->f0t[it][0].Y  = 0.001;
         base_cnst->f0t[it][0].Z  = 0.001;
      }
      base_cnst->h0t[0].X=0.; base_cnst->h0t[0].Y=0.; base_cnst->h0t[0].Z=0.;
      base_cnst->aa[0]=0.;
   }
}

} /* End of namespace */

