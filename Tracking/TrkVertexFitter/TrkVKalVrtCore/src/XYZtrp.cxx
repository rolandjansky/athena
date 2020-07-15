/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/Propagator.h"
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include <cmath>
#include <iostream>

namespace Trk {

extern const vkalPropagator  myPropagator;
extern void tdasatVK(double *, double *, double *, long int, long int);

#define cnv_ref(a_1,a_2) cnv[(a_2)*6 + (a_1) - 7]

void  xyztrp(long int *ich, double *vrt0, double *pv0, double *covi, double BMAG, double *paro, double *errt)
{
    double covd[15],par[5], cnv[36];	/* was [6][6] */ 
/* ---------------------------------------------------------- */
/*       Subroutine for convertion                            */
/*           (X,Y,Z,PX,PY,PZ) --> (eps,z,theta,phi,1/r)       */
/* Now it's used in VKalVrtFitter only                        */
/*                                                            */
/*       Input:                                               */
/*                 ICH      -  charge of track ( +1,0,-1 )    */
/*                 VRT0(3)  -  Vertex of particle             */
/*                  PV0(3)  -  Momentum of particle           */
/*                 COVI(21) -  simmetric covariance matrix    */
/*       Output:                                              */
/*                 PARO(5)  -  (eps,z,theta,phi,1/r)          */
/*                 ERRT(15) -  simmetric error matrix         */
/*    Reference point for output is (0,0,0)                   */
/*    For ICH=0  PARO(5) = const/pt                           */
/*          Propagation is done for NEW track,                */
/*           so no TrkID reference established                */
/* Author: V.Kostyukhin                                       */
/* ---------------------------------------------------------- */

    double constBF =BMAG * vkalMagCnvCst; 

    double pt = sqrt(pv0[0]*pv0[0] + pv0[1]*pv0[1]);
    double pp = pt*pt + pv0[2]*pv0[2];                   // p**2
    double cs  = pv0[0] / pt;
    double sn  = pv0[1] / pt;
    double ctg = pv0[2] / pt;
    double rho = (*ich) * constBF / pt;
    if ((*ich) == 0)rho = constBF / pt;
/* --  Output parameters */
    par[0] = 0.;                    /*            (-Yv*cos + Xv*sin) */
    par[1] = 0.;                    /*  Zv - cotth*(Xv*cos + Yv*sin) */
    par[2] = acos(pv0[2] / sqrt(pp));
    if(par[2]<1.e-5)par[2]=1.e-5;
    if(par[2]>M_PI-1.e-5) par[2]=M_PI-1.e-5;
    par[3] = atan2(pv0[1], pv0[0]);
    par[4] = rho;
    if ((*ich) == 0)par[4] = constBF / pt;
//---
    double dTheta_dPx =  pv0[0]*pv0[2]/(pt*pp);   //dTheta/dPx
    double dTheta_dPy =  pv0[1]*pv0[2]/(pt*pp);   //dTheta/dPy
    double dTheta_dPz = -pt/pp;                   //dTheta/dPz
    double dPhi_dPx   = -pv0[1]/(pt*pt);          //dPhi/dPx
    double dPhi_dPy   =  pv0[0]/(pt*pt);          //dPhi/dPy
    double dPhi_dPz   =             0;            //dPhi/dPz
    double dRho_dPx   = -pv0[0]/(pt*pt) * rho;    //dInvR/dPx
    double dRho_dPy   = -pv0[1]/(pt*pt) * rho;    //dInvR/dPy
    double dRho_dPz   =                    0.;    //dInvR/dPz
//---
    cnv_ref(1, 1) =  sn;
    cnv_ref(2, 1) = -cs;
    cnv_ref(3, 1) =  0.;
    cnv_ref(4, 1) =  0.;
    cnv_ref(5, 1) =  0.;
    cnv_ref(6, 1) =  0.;

    cnv_ref(1, 2) = -cs * ctg;
    cnv_ref(2, 2) = -sn * ctg;
    cnv_ref(3, 2) = 1.;
    cnv_ref(4, 2) = 0.;
    cnv_ref(5, 2) = 0.;
    cnv_ref(6, 2) = 0.;

    cnv_ref(1, 3) = 0.;
    cnv_ref(2, 3) = 0.;
    cnv_ref(3, 3) = 0.;
    cnv_ref(4, 3) = dTheta_dPx;
    cnv_ref(5, 3) = dTheta_dPy;
    cnv_ref(6, 3) = dTheta_dPz;

    cnv_ref(1, 4) = 0.;
    cnv_ref(2, 4) = 0.;
    if(*ich) cnv_ref(1, 4) = -cs * rho;  // For charged tracks only
    if(*ich) cnv_ref(2, 4) = -sn * rho;  //
    cnv_ref(3, 4) = 0.;
    cnv_ref(4, 4) = dPhi_dPx;
    cnv_ref(5, 4) = dPhi_dPy;
    cnv_ref(6, 4) = dPhi_dPz;

    cnv_ref(1, 5) = 0.;
    cnv_ref(2, 5) = 0.;
    cnv_ref(3, 5) = 0.;
    cnv_ref(4, 5) = dRho_dPx;
    cnv_ref(5, 5) = dRho_dPy;
    cnv_ref(6, 5) = dRho_dPz;
    tdasatVK(cnv, covi , covd, 5, 6);

/* -- Translation to (0,0,0) (BackPropagation) --*/
    double Ref0[3]={0.,0.,0.};
    myPropagator.Propagate(-999, (*ich), par, covd, vrt0, Ref0, paro, errt, nullptr);

} 


void  combinedTrack(long int ICH, double *pv0, double *covi, double BMAG, double *par, double *covo)
{

    double cnv[36];	/* was [6][6] */
/* ---------------------------------------------------------- */
/*       Subroutine for convertion for VKalvrtCore            */
/* Correct magnetic field BMAG at conversion point            */
/*  is provided externally.                                   */
/*           (X,Y,Z,PX,PY,PZ) --> (eps,z,theta,phi,1/r)       */
/*       Input:                                               */
/*                 ICH      -  charge of track ( +1,0,-1 )    */
/*                  PV0(3)  -  Momentum of particle           */
/*                 COVI(21) -  simmetric covariance matrix    */
/*       Output:                                              */
/*                 PARO(5)  -  (eps,z,theta,phi,1/r)          */
/*                 ERRT(15) -  simmetric error matrix         */
/*       NO PROPAGATION IS DONE!!!!                           */
/*    For ICH=0  PARO(5) = const/pt                           */
/* Author: V.Kostyukhin                                       */
/* ---------------------------------------------------------- */

    double constBF =BMAG * vkalMagCnvCst;

    double pt = sqrt(pv0[0]*pv0[0] + pv0[1]*pv0[1]);
    double pp = pt*pt + pv0[2]*pv0[2];
    double cs  = pv0[0] / pt;
    double sn  = pv0[1] / pt;
    double ctg = pv0[2] / pt;
    double rho = ICH * constBF / pt;
    if ( ICH==0 )rho = constBF / pt;
/* --  Output parameters */
    par[0] = 0.;                    /*            (-Yv*cos + Xv*sin) */
    par[1] = 0.;                    /*  Zv - cotth*(Xv*cos + Yv*sin) */
    par[2] = acos(pv0[2] / sqrt(pp));
    if(par[2]<1.e-5)par[2]=1.e-5;
    if(par[2]>M_PI-1.e-5) par[2]=M_PI-1.e-5;
    par[3] = atan2(pv0[1], pv0[0]);
    par[4] = rho;
    if ( ICH==0 )par[4] = constBF / pt;
//
    double dTheta_dPx =  pv0[0]*pv0[2]/(pt*pp);   //dTheta/dPx
    double dTheta_dPy =  pv0[1]*pv0[2]/(pt*pp);   //dTheta/dPy
    double dTheta_dPz = -pt/pp;                   //dTheta/dPz
    double dPhi_dPx   = -pv0[1]/(pt*pt);          //dPhi/dPx
    double dPhi_dPy   =  pv0[0]/(pt*pt);          //dPhi/dPy
    double dPhi_dPz   =             0;            //dPhi/dPz
    double dRho_dPx   = -pv0[0]/(pt*pt) * rho;    //dInvR/dPx
    double dRho_dPy   = -pv0[1]/(pt*pt) * rho;    //dInvR/dPy
    double dRho_dPz   =                    0.;    //dInvR/dPz
//---
    cnv_ref(1, 1) =  sn;
    cnv_ref(2, 1) = -cs;
    cnv_ref(3, 1) =  0.;
    cnv_ref(4, 1) =  0.;
    cnv_ref(5, 1) =  0.;
    cnv_ref(6, 1) =  0.;

    cnv_ref(1, 2) = -cs * ctg;
    cnv_ref(2, 2) = -sn * ctg;
    cnv_ref(3, 2) = 1.;
    cnv_ref(4, 2) = 0.;
    cnv_ref(5, 2) = 0.;
    cnv_ref(6, 2) = 0.;

    cnv_ref(1, 3) = 0.;
    cnv_ref(2, 3) = 0.;
    cnv_ref(3, 3) = 0.;
    cnv_ref(4, 3) = dTheta_dPx;
    cnv_ref(5, 3) = dTheta_dPy;
    cnv_ref(6, 3) = dTheta_dPz;

    cnv_ref(1, 4) = 0.;
    cnv_ref(2, 4) = 0.;
    if(ICH) cnv_ref(1, 4) = -cs * rho; //For charged tracks only
    if(ICH) cnv_ref(2, 4) = -sn * rho; //
    cnv_ref(3, 4) = 0.;
    cnv_ref(4, 4) = dPhi_dPx;
    cnv_ref(5, 4) = dPhi_dPy;
    cnv_ref(6, 4) = dPhi_dPz;

    cnv_ref(1, 5) = 0.;
    cnv_ref(2, 5) = 0.;
    cnv_ref(3, 5) = 0.;
    cnv_ref(4, 5) = dRho_dPx;
    cnv_ref(5, 5) = dRho_dPy;
    cnv_ref(6, 5) = dRho_dPz;
    tdasatVK(cnv, covi , covo, 5, 6);
} 
#undef cnv_ref

} /* End of VKalVrtCore namespace */

