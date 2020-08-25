/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/ForCFT.h"
#include <cmath>
#include <iostream>

namespace Trk {


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

void ForCFT::prcfit( long int *ntrk, double  *wm, double  *wmfit, double  *bmag, double  *vrt, double  *vrte) noexcept
{
    long int i__1;
    double   summ;


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

    localbmag = (*bmag);
    nmcnst = 0;
    for (int i=0; i<8; ++i) wmfit[i] = -10000.;
    summ = 0.;
    i__1 = (*ntrk)<vkalNTrkM ? (*ntrk): vkalNTrkM;
    for (int i=0; i<i__1; ++i) {
	wm[i] =  fabs(wm[i]);
	summ += wm[i];
    }
    if ((*wmfit) > summ) {
/*  Set general mass constraint based on ALL tracks */
	nmcnst = 1;
	for (int i = 0; i < vkalNTrkM; ++i) {
	    indtrkmc[0][i] = 0;
	    if (i < (*ntrk)) {indtrkmc[0][i] = 1;}
	}
	wmfit[0] = (*wmfit);
    }

    this->vrt[0] = vrt[0];
    this->vrt[1] = vrt[1];
    this->vrt[2] = vrt[2];
    this->covvrt[0] = vrte[0];
    this->covvrt[1] = vrte[1];
    this->covvrt[2] = vrte[2];
    this->covvrt[3] = vrte[3];
    this->covvrt[4] = vrte[4];
    this->covvrt[5] = vrte[5];
    irob = 0;
    IterationNumber    = 50;
    IterationPrecision = 1.e-3;
    for (int i = 0; i < vkalNTrkM; ++i) robres[i]=1.; //Safety
//
// Reset all constraints
//
useMassCnst = 0;
usePhiCnst = 0;
useThetaCnst = 0;
useAprioriVrt = 0;
usePointingCnst = 0;
usePassNear = 0;
//forcft_1.usePlaneCnst = 0;   //Used only on demand=> must NOT be reset here!!!
} 


void ForCFT::vksetIterationNum(long int Iter) noexcept
{
    if (Iter<3)   Iter=3;
    if (Iter>100) Iter=100;
    IterationNumber    = Iter;
}

void ForCFT::vksetIterationPrec(double Prec) noexcept
{
    if (Prec<1.e-5)   Prec=1.e-5;
    if (Prec>1.e-1)   Prec=1.e-1;
    IterationPrecision = Prec;
}

void ForCFT::vksetRobustScale(double Scale) noexcept
{
    if (Scale<0.01)   Scale=0.01;
    if (Scale>100.)   Scale=100.;
    RobustScale = Scale;
}

void ForCFT::vksetRobustness(long int Rob) noexcept
{
    if (Rob<0)   Rob=0;
    if (Rob>7)   Rob=7;
    irob = Rob;
}

void ForCFT::vksetUseMassCnst() noexcept { useMassCnst = 1;}
void ForCFT::vksetUsePhiCnst()  noexcept { usePhiCnst = 1;}
void ForCFT::vksetUsePlaneCnst(double a, double b, double c, double d) noexcept  { 
  if(a+b+c+d == 0.){  usePlaneCnst = 0;
  }else{              usePlaneCnst = 1; }
  Ap = a; Bp = b; Cp = c; Dp = d;
}



void ForCFT::setmasscnst_(long int *ncnsttrk, long int *indextrk, double  *wmcnst) noexcept
{
    if (indextrk==nullptr) return;  //Protection!  Track indices start from 1 (not 0)!
    --indextrk;

    ++nmcnst;
    if (nmcnst > 8) return ;
    for (int i = 0; i < vkalNTrkM; ++i) {
	indtrkmc[nmcnst-1][i] = 0;
    }
    for (int i = 0; i < (*ncnsttrk);  ++i) {
	if (indextrk[i] > 0 && indextrk[i] < vkalNTrkM) {
	    indtrkmc[nmcnst-1][indextrk[i]] = 1;
	}
    }
    wmfit[nmcnst - 1] = (*wmcnst);
 }

} /* End of namespace */
