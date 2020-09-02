/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Calculates COVF(21) - symmetric 6x6 covariance matrix
// for combined (X,Y,Z,Px,Py,Pz) vector after vertex fit 
//
//-----------------------------------------------------
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include <cmath>

namespace Trk {

//
//  Function calculates complete error matrix ADER and derivatives 
//   of fitted track parameters with respect to initial ones.
//    It calculates also combined momentum and (VxVyVzPxPyPz) covarinace
//  Complete error matrix is recalculated here via getFullVrtCov,
//            so CPU CONSUMING!!!
//--------------------------------------------------------------
extern int getFullVrtCov(VKVertex *, double *, double *, double[6][6]);
extern void cfsetdiag(long int , double *, double );
extern const vkalMagFld      myMagFld;

int afterFit(VKVertex *vk, double *ader, double * dcv, double * ptot, double * VrtMomCov, const VKalVrtControlBase* CONTROL )
{
    int i,j;
    double px,py,pz,pt,invR,cth;
    double verr[6][6]={{0.}};   //for (i=0; i<6*6; i++) verr[i]=0;

    int NTRK = vk->TrackList.size();
    int NVar = NTRK*3+3;
    for (i=1; i<=6; ++i) {
	for (j=1; j<=NVar  ; ++j) dcv[i + j*6 - 7] = 0.;
    }
    cfsetdiag( 6, VrtMomCov, 100.);
    ptot[0] = 0.;
    ptot[1] = 0.;
    ptot[2] = 0.;
    double constBF = myMagFld.getMagFld(vk->refIterV,CONTROL) * myMagFld.getCnvCst() ;

    for (i = 1; i <= NTRK; ++i) {
        VKTrack *trk=vk->TrackList[i-1].get();
        invR = trk->fitP[2];
	cth = 1. / tan(trk->fitP[0]);
	pt = constBF / fabs(invR);
	px = pt * cos(trk->fitP[1]);
	py = pt * sin(trk->fitP[1]);
	pz = pt * cth;
	ptot[0] += px;
	ptot[1] += py;
	ptot[2] += pz;
	dcv[ (i*3 + 0)*6 + 5] = -pt * (cth * cth + 1);     /* dPz/d(theta) */
	dcv[ (i*3 + 1)*6 + 3] = -py;                       /* dPx/d(phi) */
	dcv[ (i*3 + 1)*6 + 4] =  px;                       /* dPy/d(phi) */
	dcv[ (i*3 + 2)*6 + 3] = -px / invR; 	   	   /* dPx/d(rho) */
	dcv[ (i*3 + 2)*6 + 4] = -py / invR;  	  	   /* dPy/d(rho) */
	dcv[ (i*3 + 2)*6 + 5] = -pz / invR;  	           /* dPz/d(rho) */
    }
    dcv[0]  = 1.;
    dcv[7]  = 1.;
    dcv[14] = 1.;
    if(!ader)return 0;
    int IERR=getFullVrtCov(vk, ader, dcv, verr);     if (IERR) return IERR;
    int ijk = 0;
    for ( i=0; i<6; ++i) {
	for (j=0; j<=i; ++j) {
	    VrtMomCov[ijk++] = verr[i][j];
	}
    }
    return 0;
}

//   The same as afterFit(), but instead of fitted track parameters 
//          it uses guessed track parameters. ( it's needed for cascade)
//  Complete error matrix is recalculated here via getFullVrtCov,
//            so CPU CONSUMING!!!
//--------------------------------------------------------------------------------------------------
int afterFitWithIniPar(VKVertex *vk, double *ader, double * dcv, double * ptot, double * VrtMomCov, const VKalVrtControlBase* CONTROL  )
{
    int i,j;
    double px,py,pz,pt,invR,cth;
    double verr[6][6]={{0.}};


    int NTRK = vk->TrackList.size();
    int NVar = NTRK*3+3;
    for (i=1; i<=6; ++i) {
	for (j=1; j<=NVar  ; ++j)  dcv[i + j*6 - 7] = 0.;
    }
    cfsetdiag( 6, VrtMomCov, 10.);
    ptot[0] = 0.;
    ptot[1] = 0.;
    ptot[2] = 0.;
    double constBF = myMagFld.getMagFld(vk->refIterV,CONTROL) * myMagFld.getCnvCst() ;

    for (i = 1; i <= NTRK; ++i) {
        VKTrack *trk=vk->TrackList[i-1].get();
        invR = trk->iniP[2];
	cth = 1. / tan(trk->iniP[0]);
	pt = constBF / fabs(invR);
	px = pt * cos(trk->iniP[1]);
	py = pt * sin(trk->iniP[1]);
	pz = pt * cth;
	ptot[0] += px;
	ptot[1] += py;
	ptot[2] += pz;
	dcv[ (i*3 + 0)*6 + 5] = -pt * (cth * cth + 1);     /* dPz/d(theta) */
	dcv[ (i*3 + 1)*6 + 3] = -py;                       /* dPx/d(phi) */
	dcv[ (i*3 + 1)*6 + 4] =  px;                       /* dPy/d(phi) */
	dcv[ (i*3 + 2)*6 + 3] = -px / invR; 	   	   /* dPx/d(rho) */
	dcv[ (i*3 + 2)*6 + 4] = -py / invR;  	  	   /* dPy/d(rho) */
	dcv[ (i*3 + 2)*6 + 5] = -pz / invR;  	           /* dPz/d(rho) */
    }
    dcv[0]  = 1.;
    dcv[7]  = 1.;
    dcv[14] = 1.;
    if(!ader)return 0;
    int IERR=getFullVrtCov(vk, ader, dcv, verr);     if (IERR) return IERR;
    int ijk = 0;
    for ( i=0; i<6; ++i) {
	for (j=0; j<=i; ++j) {
	    VrtMomCov[ijk++] = verr[i][j];
	}
    }
    return 0;
}

} /* End of namespace */


