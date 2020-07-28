/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/Propagator.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include <cmath>
#include <iostream>


namespace Trk {

extern const vkalPropagator  myPropagator;
extern const vkalMagFld      myMagFld;


void vpderiv(bool UseTrackErr, long int Charge, double *pari0, double *covi, double *vrtref, double *covvrtref, 
      double *drdpar, double *dwgt, double *rv0, const VKalVrtControl * FitCONTROL)
{
    /* Initialized data */

    double dshft[6] = { 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4};   //step for each parameter
         /* Symmetric version for derivatives calculation*/
    double DrvCoef[4] = {-2., -1., 1.,2.};

    /* Local variables */
    double pari[6], covd[15], dcov[3], rvec[50]; /* was [2][6*4+1] */
    double  paro[5];
    int j, ij, ip, ipp, id=0;
    double dwgt0[3]={0.,0.,0.};
    //double deriv[6], dchi2[4*6+1];                     //VK for debugging
    double cs, pp, sn, pt, rho;
    double covdtr[15], ctg, par[5], cnv[36];	/* was [6][6] */


    extern int cfdinv(double *, double *, long int);
    extern void tdasatVK(double *, double *, double *, long int, long int);


#define rvec_ref(a_1,a_2) rvec[(a_2)*2 + (a_1) - 1]
#define drdpar_ref(a_1,a_2) drdpar[(a_2)*2 + (a_1)]
#define cnv_ref(a_1,a_2) cnv[(a_2)*6 + (a_1) - 7]

/* ---------------------------------------------------------- */
/*       Subroutine for derivative calculations               */
/*       Input:                                               */
/*              Charge         -  charge of track ( +1,0,-1 ) */
/*              PARI0(1:3)  -  Vertex of particle             */
/*              PARI0(4:6)  -  Momentum of particle           */
/*                COVI(21)  -  symmetric covariance matrix    */
/* 		  BMAG      -  magnetic field                 */
/*     UseTrackErr =false  Fitted track is assumed errorless  */
/*                         only vertex error is used          */
/*                          may be used for constraint        */        
/*                  =true  Fitted track error is added        */
/*       Output:                                              */
/*                Deriv(6)  -  Chi2 derivatives for debugging */
/*                drdpar    -  derivatives                    */
/*                rv0[2]    -  track impact vs vertex         */
/*                dwgt[3]   -  track impact covarinance       */
/* ---------------------------------------------------------- */
    /* Parameter adjustments */
    drdpar -= 3;
    --covvrtref;

    /* Function Body */
/* --------------------- */
    double constB =FitCONTROL->vk_forcft.localbmag * vkalMagCnvCst ;
    
    for (ip = 0; ip <= 4*6; ++ip) {    // Number of points * Number of parameters
/* --  Input parameters */
	pari[0] = pari0[0];
	pari[1] = pari0[1];
	pari[2] = pari0[2];
	pari[3] = pari0[3];
	pari[4] = pari0[4];
	pari[5] = pari0[5];
//  ipp - parameter number, id - number of step
	ipp = ip; while(ipp>6)ipp-=6;
	if(ip>=1  && ip<=6 ) id=0;
	if(ip>=7  && ip<=12) id=1;
	if(ip>=13 && ip<=18) id=2;
	if(ip>=19 && ip<=24) id=3;
	if (ip != 0) pari[ipp - 1] *= (dshft[ipp-1]*DrvCoef[id] + 1.);
/* -- */
	pt  = sqrt(pari[3]*pari[3] + pari[4]*pari[4]);
	pp  = pt*pt + pari[5]*pari[5];
	cs  = pari[3] / pt;
	sn  = pari[4] / pt;
	ctg = pari[5] / pt;
	rho =  Charge * constB / pt;   // Sign of charge coinside with sign of curvature!!!
/* --  Output parameters */
	par[0] = 0.;
	par[1] = 0.;
	par[2] = acos(pari[5] / sqrt(pp));
        if(par[2]<1.e-5) par[2]=1.e-5;
        if(par[2]>M_PI-1.e-5) par[2]=M_PI-1.e-5;
	par[3] = atan2(pari[4], pari[3]);
	par[4] = rho;
	if (Charge == 0)  par[4] = constB / pt;
/* -- */
	cnv_ref(1, 1) = sn;
	cnv_ref(2, 1) = -cs;
	cnv_ref(3, 1) = 0.;
	cnv_ref(4, 1) = 0.;
	cnv_ref(5, 1) = 0.;
	cnv_ref(6, 1) = 0.;
/* -- */
	cnv_ref(1, 2) = -cs * ctg;
	cnv_ref(2, 2) = -sn * ctg;
	cnv_ref(3, 2) = 1.;
	cnv_ref(4, 2) = 0.;
	cnv_ref(5, 2) = 0.;
	cnv_ref(6, 2) = 0.;
/* -- */
	cnv_ref(1, 3) = 0.;
	cnv_ref(2, 3) = 0.;
	cnv_ref(3, 3) = 0.;
	cnv_ref(4, 3) = pari[5] * pari[3] / pp / pt;
	cnv_ref(5, 3) = pari[5] * pari[4] / pp / pt;
	cnv_ref(6, 3) = -pt / pp;
/* -- */
	cnv_ref(1, 4) = 0.;
	cnv_ref(2, 4) = 0.;
        if (Charge)cnv_ref(1, 4) = -cs * rho;   //  For charged tracks only
        if (Charge)cnv_ref(2, 4) = -sn * rho;   //
	cnv_ref(3, 4) = 0.;
	cnv_ref(4, 4) = -pari[4] / (pt * pt);
	cnv_ref(5, 4) =  pari[3] / (pt * pt);
	cnv_ref(6, 4) = 0.;
/* -- */
	cnv_ref(1, 5) = 0.;
	cnv_ref(2, 5) = 0.;
	cnv_ref(3, 5) = 0.;
	cnv_ref(4, 5) = -pari[3] * rho / (pt * pt);
	cnv_ref(5, 5) = -pari[4] * rho / (pt * pt);
	if ( Charge == 0) {cnv_ref(4, 5) = -pari[3] * constB / (pt*pt*pt);}
	if ( Charge == 0) {cnv_ref(5, 5) = -pari[4] * constB / (pt*pt*pt);}
	cnv_ref(6, 5) = 0.;
/* -- not needed really but... */
	cnv_ref(1, 6) = cnv_ref(2, 6) = cnv_ref(3, 6) = cnv_ref(4, 6) = cnv_ref(5, 6) = cnv_ref(6, 6) = 0.;

	tdasatVK(cnv, &covi[0], covd, 5, 6);

/* -- Translation to New Reference Point */
        myPropagator.Propagate(-999, Charge, par, covd, pari, vrtref, paro, covdtr, FitCONTROL);



/* -- Now impact significance */
/*  X=paro(1)*sn, Y=-paro(1)*cs */
	sn = sin(paro[3]);
	cs = cos(paro[3]);
/* -- New error version */
	cnv_ref(1, 1) = -sn;
	cnv_ref(1, 2) =  cs;
	cnv_ref(1, 3) =  0.;
	cnv_ref(2, 1) =  sn / tan(paro[2]);
	cnv_ref(2, 2) =  cs / tan(paro[2]);
	cnv_ref(2, 3) = -1.;
	dcov[0] = 0.;
	dcov[1] = 0.;
	dcov[2] = 0.;
	for (int i = 1; i <= 3; ++i) {
	    for (j = 1; j <= 3; ++j) {
	        ij = i > j ? i : j;
		ij = ij * (ij - 1) / 2 + ( i < j ? i : j );
		dcov[0] += cnv_ref(1, i) * cnv_ref(1, j) * covvrtref[ij];
		dcov[2] += cnv_ref(2, i) * cnv_ref(2, j) * covvrtref[ij];
		dcov[1] += cnv_ref(1, i) * cnv_ref(2, j) * covvrtref[ij];
	    }
	}
/* --------------------------------------------------------------- */
	if( UseTrackErr ){          // Add track error if needed
	   dcov[0] += covdtr[0];
	   dcov[1] += covdtr[1];
	   dcov[2] += covdtr[2];
        }
/*---------------------------------------------------------------- */	
//     Weight matrix and chi2 for given shift	
//
	int jerr=cfdinv(dcov, &dwgt[0], -2); if(jerr){jerr=cfdinv(dcov, &dwgt[0], 2); if(jerr){dwgt[0]=dwgt[2]=1.e6; dwgt[1]=0.;}};
	//dchi2[ip] = sqrt(fabs(dwgt[0]*paro[0]*paro[0] + 2.*dwgt[1]*paro[0]*paro[1] + dwgt[2]*paro[1]*paro[1]));
	rvec_ref(1, ip) = paro[0];
	rvec_ref(2, ip) = paro[1];
	if (ip == 0) {          //Save weight matrix for output
	    dwgt0[0] = dwgt[0];
	    dwgt0[1] = dwgt[1];
	    dwgt0[2] = dwgt[2];
	}
    }
/* ---------------------------------------------------------------- */
//  Now calculate derivatives themselves
//
    for (int i = 1; i <= 6; ++i) {
      //deriv[i-1]      = (dchi2[i] -8.*dchi2[i+6] +8.*dchi2[i+12]-dchi2[i+18] ) / (12.*dshft[i-1]*pari0[i-1]);
      drdpar_ref(1,i) = (rvec_ref(1,i) -8.*rvec_ref(1,i+6) +8.*rvec_ref(1,i+12) -rvec_ref(1,i+18)) / (12.*dshft[i-1]*pari0[i-1]);
      drdpar_ref(2,i) = (rvec_ref(2,i) -8.*rvec_ref(2,i+6) +8.*rvec_ref(2,i+12) -rvec_ref(2,i+18)) / (12.*dshft[i-1]*pari0[i-1]);
    }
//std::cout<<deriv[0]<<", "<<deriv[1]<<", "<<deriv[2]<<", "<<deriv[3]<<", "<<deriv[4]<<", "<<deriv[5]<<'\n';
//std::cout<<drdpar_ref(1, 1)<<", "<<drdpar_ref(1, 2)<<", "<<drdpar_ref(1, 3)<<", "<<drdpar_ref(1, 4)<<
//                             ", "<<drdpar_ref(1, 5)<<", "<<drdpar_ref(1, 6)<<'\n';
//std::cout<<drdpar_ref(2, 1)<<", "<<drdpar_ref(2, 2)<<", "<<drdpar_ref(2, 3)<<", "<<drdpar_ref(1, 4)<<
//                             ", "<<drdpar_ref(2, 5)<<", "<<drdpar_ref(2, 6)<<'\n';
//std::cout<<"---------------------"<<'\n';
//
//  Output parameters
//

    rv0[0] = rvec_ref(1, 0);
    rv0[1] = rvec_ref(2, 0);
    dwgt[0] = dwgt0[0];
    dwgt[1] = dwgt0[1];
    dwgt[2] = dwgt0[2];

//std::cout<<" VpDerivR,Z="<<rv0[0]<<", "<<rv0[1]<<'\n';
//std::cout<<" Ref="<<vrtref[0]<<", "<<vrtref[1]<<", "<<vrtref[2]<<'\n';
} 
#undef cnv_ref
#undef drdpar_ref
#undef rvec_ref

} /* End of namespace */
