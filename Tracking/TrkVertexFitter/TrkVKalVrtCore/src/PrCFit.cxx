/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include "TrkVKalVrtCore/ForCFT.h"
#include "TrkVKalVrtCore/CommonPars.h"
#include <iostream>

namespace Trk {

extern ForCFT      forcft_;

#define forcft_1 forcft_

/*------------------------------------------------------------------------------*/
/* Package for vertex fit with constraints 					*/
/*   Minimal calling sequence: 							*/
/*   .......................... 						*/
/*   call PRCFIT(ntrk,wm,bmag,vrt,vrte,irob):   setting values for cnsts 	*/
/*   call SetMassCnst(bmag,vrt,vrte,irob):   setting values for cnsts 		*/
/*  call CFPEST(ntrk,xyz0,ich,apar,par0)         : primary estimation of Theta, */
/*   				                 : phi,1/r if they are not known*/
/*   call CFIT(iflag,ifcovv0,ntrk,ich,xyz0,par0,apar,awgt,  : fit itself 	*/
/*  @  xyzfit,parfs,ptot,covf,chi2,chi2tr,ierr) 				*/
/*   if(ierr.ne.0)  goto 9999 							*/
/*   ........................... 						*/
/*   call FITERM(ntrk,errmtx)       : full error matrix  in symmetric 		*/
/* 			           : form ( if needed) 				*/
/*         ........................... 						*/
/*   WARNING 1:  For using tracks with charge=0 they should have 		*/
/*               par(5) = MAG*0.002998/pt and correct definition 		*/
/*               of weight matrix ( or at least weight(15)=1./err**2) 		*/
/*   WARNING 2:  Magnetic field in ATLAS for use in fit is 2.084 ( centre )	*/
/*   WARNING 3:  Perigee parameters inside package DOESN'T coinside with 	*/
/*               those in XKALMAN. See conversion code in GetTrkPar and 	*/
/*               CnvErrMtx subroutines. (Will be changed in future...) 		*/
/*   Other useful subroutines (see comments inside code): 			*/
/*   XYZTRP :   track conversion (X,Y,Z,PX,PY,PZ) --> (eps,z,theta,phi,1/r)	*/
/*   CFIMP  :   track impact parameters calculation 				*/
/*   CFNEWP :   propagate track to new vertex 					*/
/*   CFERPR :   propagate track error matrix to new vertex 			*/
/*   STPCNV :   track momentum from track (theta,phi,1/r) at vertex       	*/
/*------------------------------------------------------------------------------*/

void prcfit( long int *ntrk, double  *wm, double  *wmfit, double  *bmag, double  *vrt, double  *vrte)
{
    long int i__1;
    double   summ;

#define indtrkmc_ref(a_1,a_2) forcft_1.indtrkmc[(a_2)*NTrkM + (a_1) - (NTrkM+1)]

/*------------------------------------------------------------------------------*/
/*        SETTING OF INITIAL PARAMETERS FOR C-FIT 				*/
/*        (DEPENDING ON TYPE OF FIT) 						*/
/*          INPUT: 								*/
/* 		   NTRK        : number of tracks 				*/
/*                 WM(JTK)     : track masses ( if negative - track doesn't	*/
/* 			       : participate in mass constraint ) 		*/
/*                 WMFIT       : masses for mass constraints 			*/
/*                 VRT(3)      : point for directional constraint 		*/
/*                                        (usually primary vertex) 		*/
/*                 VRTE(6)     : error matrix for VRT ( for IFLAG=6)		*/
/*                 BMAG        : magnetic field (in Tesla)	 		*/
/*    Magnetic field is obligatory , other parameters MUST be set		*/
/*    when used in constraints 							*/
/*     General mass constraint for all tracks is set in PRCFIT through WMFIT	*/
/*    if WMFIT > Sum_of_track_masses 						*/
/*       Any number of additional mass constraints can be set up with 		*/
/*     SetMassCnst(NCnstTrk,IndexTrk,WMCNST)     where 				*/
/*               INPUT: 							*/
/*                    NCnstTrk           :track number of track subset for cnst.*/
/*                     IndexTrk(NCnstTrk) :index of each track from subset in	*/
/*                                         :general track set for vertex fit.	*/
/*                       WmCnst             :Mass for given constraint 		*/
/* Author: V.Kostioukhine (1995 ,2004,2005) 					*/
/*									    	*/
/*		All functions here must be called AFTER(!!!) prcfit.		*/
/*										*/
/*------------------------------------------------------------------------------*/

    forcft_1.localbmag = (*bmag);
    forcft_1.nmcnst = 0;
    for (int i=0; i<8; ++i) forcft_1.wmfit[i] = -10000.;
    summ = 0.;
    i__1 = (*ntrk)<NTrkM ? (*ntrk): NTrkM;
    for (int i=0; i<i__1; ++i) {
	forcft_1.wm[i] =  fabs(wm[i]);
	summ += wm[i];
    }
    if ((*wmfit) > summ) {
/*  Set general mass constraint based on ALL tracks */
	forcft_1.nmcnst = 1;
	for (int i = 1; i <= NTrkM; ++i) {
	    indtrkmc_ref(i, forcft_1.nmcnst) = 0;
	    if (i <= (*ntrk)) {indtrkmc_ref(i, 1) = 1;}
	}
	forcft_1.wmfit[0] = (*wmfit);
    }

    forcft_1.vrt[0]    = vrt[0];
    forcft_1.vrt[1]    = vrt[1];
    forcft_1.vrt[2]    = vrt[2];
    forcft_1.covvrt[0] = vrte[0];
    forcft_1.covvrt[1] = vrte[1];
    forcft_1.covvrt[2] = vrte[2];
    forcft_1.covvrt[3] = vrte[3];
    forcft_1.covvrt[4] = vrte[4];
    forcft_1.covvrt[5] = vrte[5];
    forcft_1.irob = 0;
    forcft_1.IterationNumber    = 50;
    forcft_1.IterationPrecision = 1.e-3;
    for (int i = 0; i < NTrkM; ++i) forcft_1.robres[i]=1.; //Safety
//
// Reset all constraints
//
forcft_1.useMassCnst = 0;
forcft_1.usePhiCnst = 0;
forcft_1.useThetaCnst = 0;
forcft_1.useAprioriVrt = 0;
forcft_1.usePointingCnst = 0;
forcft_1.usePassNear = 0;
//forcft_1.usePlaneCnst = 0;   //Used only on demand=> must NOT be reset here!!!
} 


void vksetIterationNum(long int Iter)
{
    if (Iter<3)   Iter=3;
    if (Iter>100) Iter=100;
    forcft_1.IterationNumber    = Iter;
}

void vksetIterationPrec(double Prec)
{
    if (Prec<1.e-5)   Prec=1.e-5;
    if (Prec>1.e-1)   Prec=1.e-1;
    forcft_1.IterationPrecision = Prec;
}

void vksetRobustScale(double Scale)
{
    if (Scale<0.01)   Scale=0.01;
    if (Scale>100.)   Scale=100.;
    forcft_1.RobustScale = Scale;
}

void vksetRobustness(long int Rob)
{
    if (Rob<0)   Rob=0;
    if (Rob>7)   Rob=7;
    forcft_1.irob = Rob;
}

void vksetUseMassCnst()  { forcft_1.useMassCnst = 1;}
void vksetUsePhiCnst()   { forcft_1.usePhiCnst = 1;}
void vksetUsePlaneCnst(double a, double b, double c, double d)   { 
  if(a+b+c+d == 0.){  forcft_1.usePlaneCnst = 0;
  }else{              forcft_1.usePlaneCnst = 1; }
  forcft_1.Ap = a; forcft_1.Bp = b; forcft_1.Cp = c; forcft_1.Dp = d;
}
void vksetUseThetaCnst() { forcft_1.useThetaCnst = 1;}
void vksetUseAprioriVrt(){ forcft_1.useAprioriVrt = 1;}
void vksetUsePointingCnst(int iType = 1 ) { forcft_1.usePointingCnst = iType<2 ? 1 : 2 ;}
void vksetUsePassNear(int iType = 1 ) { forcft_1.usePassNear = iType<2 ? 1 : 2 ;}


void setmasscnst_(long int *ncnsttrk, long int *indextrk, double  *wmcnst)
{
    if (indextrk==0) return;  //Protection!  Track indices start from 1 (not 0)!
    --indextrk;

    ++forcft_1.nmcnst;
    if (forcft_1.nmcnst > 8) return ;
    for (int i = 1; i <= NTrkM; ++i) {
	indtrkmc_ref(i, forcft_1.nmcnst) = 0;
    }
    for (int i = 1; i <= (*ncnsttrk);  ++i) {
	if (indextrk[i] > 0 && indextrk[i] <= NTrkM) {
	    indtrkmc_ref(indextrk[i], forcft_1.nmcnst) = 1;
	}
    }
    forcft_1.wmfit[forcft_1.nmcnst - 1] = (*wmcnst);
 }
#undef indtrkmc_ref

} /* End of namespace */
