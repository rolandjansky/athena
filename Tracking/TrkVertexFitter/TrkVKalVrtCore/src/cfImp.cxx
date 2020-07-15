/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/Propagator.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
#include <cmath>
#include <iostream>

namespace Trk {

extern const vkalPropagator  myPropagator;
extern int cfdinv(double *, double *, long int); 

#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))


 void   cfimp(long int TrkID, long int ich, int IFL, double *par, 
	double *err, double *vrt, double *vcov, double *rimp, 
	double *rcov, double *sign , const VKalVrtControlBase * FitCONTROL )
{
    double dcov[3], errd[15], paro[5];
    double dwgt[3], errn[15];
    int i__, j, ij;

    double cnv[6]	/* was [2][3] */;
/* --------------------------------------------------------- */
/*  Impact parameter calculation                             */
/*  Input:                                                   */
/*      ICH -charge(-1,0,1)                                  */
/*      IFL = 1 contribution from VRT is added               */
/*      IFL =-1 contribution from VRT is subtructed          */
/*      PAR(5),ERR(15) - peregee parameters and error matrix */
/*      VRT(3),VCOV(6) - vertex and its error matrix         */
/*                                                           */
/*    SIGNIFICANCE IS CALCULATED FOR PERIGEE POINT BY DEF.   */
/*           (NOT FOR THE CLOSEST POINT!!!)                  */
/*                                                           */
/*  Output:                                                  */
/*     RIMP(1) - impact in XY                                */
/*     RIMP(2) - impact in Z                                 */
/*     RIMP(3) - Theta at new vertex                         */
/*     RIMP(4) - Phi at new vertex                           */
/*     RIMP(5) - curvature at vertex                         */
/*     RCOV(3) - error matrix for RIMP                       */
/*     SIGN    - impact significance                         */
/* Author: V.Kostyukhin                                      */
/* --------------------------------------------------------- */
    /* Parameter adjustments */
    --vcov;

    /* Function Body */
    for (int ii = 0; ii < 15; ++ii) {errd[ii] = err[ii];}


    double Ref0[3]={0.,0.,0.};  //base reference point for standard perigee

    myPropagator.Propagate(TrkID, ich, par, errd, Ref0, vrt, paro, errn, FitCONTROL);

//std::cout <<" CFImp new par R,Z="<<paro[0]<<", "<<paro[1]<<'\n';
/* ---------- */
    rimp[0] = paro[0];
    rimp[1] = paro[1];
    rimp[2] = paro[2];
    rimp[3] = paro[3];
    rimp[4] = paro[4];
/*  X=paro(1)*sn, Y=-paro(1)*cs */
    double sn = sin(paro[3]);
    double cs = cos(paro[3]);
/* -- New error version */
    cnv[0] = -sn;
    cnv[2] = cs;
    cnv[4] = 0.;
    cnv[1] = sn / tan(paro[2]);
    cnv[3] = cs / tan(paro[2]);
    cnv[5] = -1.;
    dcov[0] = 0.;
    dcov[1] = 0.;
    dcov[2] = 0.;
    for (i__ = 1; i__ <= 3; ++i__) {
	for (j = 1; j <= 3; ++j) {
	    ij = max(i__,j);
	    ij = ij * (ij - 1) / 2 + min(i__,j);
	    dcov[0] += cnv[(i__ << 1) - 2] * cnv[(j << 1) - 2] * vcov[ij];
	    dcov[2] += cnv[(i__ << 1) - 1] * cnv[(j << 1) - 1] * vcov[ij];
	    dcov[1] += cnv[(i__ << 1) - 2] * cnv[(j << 1) - 1] * vcov[ij];
	}
    }
/* --------------------------------------------------------------- */
    if (IFL == 1) {
	rcov[0] = errn[0] + dcov[0];
	rcov[1] = errn[1] + dcov[1];
	rcov[2] = errn[2] + dcov[2];
    } else if (IFL == -1) {
	rcov[0] = errn[0] - dcov[0];
	rcov[1] = errn[1] - dcov[1];
	rcov[2] = errn[2] - dcov[2];
    } else {
	rcov[0] = errn[0];
	rcov[1] = errn[1];
	rcov[2] = errn[2];
    }
    int jerr=cfdinv(rcov, dwgt, -2);
    if (jerr) {jerr=cfdinv(rcov, dwgt, 2); if(jerr){dwgt[0]=dwgt[2]=1.e6; dwgt[1]=0.;}}
    (*sign) = sqrt(fabs(dwgt[0] * rimp[0] * rimp[0] + dwgt[1] * 2. * rimp[0] * rimp[1] + 
	    dwgt[2] * rimp[1] * rimp[1]));
} 


 void   cfimpc(long int TrkID, long int ich, int IFL, double *par, 
	double *err, double *vrt, double *vcov, double *rimp, 
	double *rcov, double *sign, const VKalVrtControlBase * FitCONTROL )
{
    double dcov[3], errd[15], paro[5];
    double dwgt[3], errn[15], cnv[6];	/* was [2][3] */
    int i__, j, ij;


    double cs, sn;
    extern void cfClstPnt(double *p, double *, double *);
/* --------------------------------------------------------- */
/*    SIGNIFICANCE IS CALCULATED FOR THE CLOSEST POINT NOW!!!*/
/* Author: V.Kostyukhin                                      */
/* --------------------------------------------------------- */
    /* Parameter adjustments */
    --vcov;
    for (int ii = 0; ii < 15; ++ii) {errd[ii] = err[ii];}

    double Ref0[3]={0.,0.,0.};  //base reference point for standard perigee
    myPropagator.Propagate(TrkID, ich, par, errd, Ref0, vrt, paro, errn, FitCONTROL);

    double tmpVrt[3]={0.,0.,0.}; double ClosestPnt[3];
    cfClstPnt( paro, tmpVrt, ClosestPnt);
    paro[0]=sqrt(ClosestPnt[0]*ClosestPnt[0] + ClosestPnt[1]*ClosestPnt[1]);
    paro[1]=ClosestPnt[2];
/* ---------- */
    rimp[0] = paro[0];
    rimp[1] = paro[1];
    rimp[2] = paro[2];
    rimp[3] = paro[3];
    rimp[4] = paro[4];
    sn = sin(paro[3]);
    cs = cos(paro[3]);
/* -- New error version */
    cnv[0] = -sn;
    cnv[2] = cs;
    cnv[4] = 0.;
    cnv[1] = sn / tan(paro[2]);
    cnv[3] = cs / tan(paro[2]);
    cnv[5] = -1.;
    dcov[0] = 0.;
    dcov[1] = 0.;
    dcov[2] = 0.;
    for (i__ = 1; i__ <= 3; ++i__) {
	for (j = 1; j <= 3; ++j) {
	    ij = max(i__,j);
	    ij = ij * (ij - 1) / 2 + min(i__,j);
	    dcov[0] += cnv[(i__ << 1) - 2] * cnv[(j << 1) - 2] * vcov[ij];
	    dcov[2] += cnv[(i__ << 1) - 1] * cnv[(j << 1) - 1] * vcov[ij];
	    dcov[1] += cnv[(i__ << 1) - 2] * cnv[(j << 1) - 1] * vcov[ij];
	}
    }
/* --------------------------------------------------------------- */
    if (IFL == 1) {
	rcov[0] = errn[0] + dcov[0];
	rcov[1] = errn[1] + dcov[1];
	rcov[2] = errn[2] + dcov[2];
    } else if (IFL == -1) {
	rcov[0] = errn[0] - dcov[0];
	rcov[1] = errn[1] - dcov[1];
	rcov[2] = errn[2] - dcov[2];
    } else {
	rcov[0] = errn[0];
	rcov[1] = errn[1];
	rcov[2] = errn[2];
    }
    int jerr=cfdinv(rcov, dwgt, -2);
    if (jerr) {jerr=cfdinv(rcov, dwgt, 2);if(jerr){dwgt[0]=dwgt[2]=1.e6; dwgt[1]=0.;}}
    (*sign) = sqrt(fabs(dwgt[0] * rimp[0] * rimp[0] + dwgt[1] * 2. * rimp[0] * rimp[1] + 
	    dwgt[2] * rimp[1] * rimp[1]));
} 

} /* end of namespace */

