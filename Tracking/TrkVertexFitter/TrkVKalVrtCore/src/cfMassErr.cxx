/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"

namespace Trk {


void cfmasserr(VKVertex * vk, int *list, double BMAG, double *MASS, double *sigM)
{
  int NTRK=vk->TrackList.size();
  auto deriv = std::make_unique< double[] >(3*NTRK+3);
  double ptot[4]={0.};
  std::vector< std::array<double,6> > pmom(NTRK);
  double dm2dpx, dm2dpy, dm2dpz, ee, pt, px, py, pz, cth;

  double constBF = BMAG * vkalMagCnvCst ;

  for(int it=0; it<NTRK; it++){
    if (list[it]) {
       pmom[it][4] = pt = constBF / fabs(vk->TrackList[it]->fitP[2]);
       pmom[it][5] = cth = 1. /tan(vk->TrackList[it]->fitP[0]);
       pmom[it][0] = px = pt * cos(vk->TrackList[it]->fitP[1]);
       pmom[it][1] = py = pt * sin(vk->TrackList[it]->fitP[1]);
       pmom[it][2] = pz = pt * cth;
       double mmm=vk->TrackList[it]->getMass();
       pmom[it][3] = sqrt(px*px + py*py + pz*pz + mmm*mmm);
       ptot[0] += px;
       ptot[1] += py;
       ptot[2] += pz;
       ptot[3] += pmom[it][3];   
    }
  }

  for(int it = 0; it < NTRK; ++it) {
    if (list[it]) {
       pt = pmom[it][4];
       cth= pmom[it][5];
       px = pmom[it][0];
       py = pmom[it][1];
       pz = pmom[it][2];
       ee = pmom[it][3]; 
       dm2dpx = (ptot[3] / ee * px - ptot[0]) * 2.;
       dm2dpy = (ptot[3] / ee * py - ptot[1]) * 2.;
       dm2dpz = (ptot[3] / ee * pz - ptot[2]) * 2.;
       deriv[it*3 + 0] = dm2dpz * ((-pt) * (cth * cth + 1.));                                /* d(M2)/d(Theta) */
       deriv[it*3 + 1] =  -dm2dpx * py + dm2dpy * px;                                        /* d(M2)/d(Phi)   */
       deriv[it*3 + 2] = (-dm2dpx * px - dm2dpy*py - dm2dpz*pz)/vk->TrackList[it]->fitP[2];  /* d(M2)/d(Rho)   */
    } else {
       deriv[it*3 + 0] = deriv[it*3 + 1] = deriv[it*3 + 2] = 0.;
    }
  }
//----
  double covM2=0;
  for(int i=0; i<NTRK*3; i++){
     for(int j=0; j<NTRK*3; j++){
       covM2 += deriv[i] * ARR2D_FS(vk->ader, vkalNTrkM*3+3, i+3, j+3) *deriv[j];
     }
  }
//----
  (*MASS) = (ptot[3]-ptot[2])*(ptot[3]+ptot[2])-ptot[1]*ptot[1]-ptot[0]*ptot[0];
  if((*MASS)<1.e-10)  (*MASS) = 1.e-10;
  (*MASS)   = sqrt(*MASS);
  (*sigM) = sqrt(covM2) / 2. / (*MASS);
  return;
}


//#define ader_ref(a_1,a_2) workarray_.ader[(a_2)*(vkalNTrkM*3+3) + (a_1) - (vkalNTrkM*3+4)]
/* -------------------------------------------------------------- */
/*  RETURN FULL ERROR MATRIX AFTER THE FIT                        */
/*  ERRMTX SHOULD HAVE AT LEAST (3*NTRK+3)*(3*NTRK+4)/2. ELEMENTS */
/* ------------------------------------------------------- */
/*  RETURN ERROR OF ANY VARIABLE AFTER THE FIT */
/*void cferrany_(long int *ntrk, double  *deriv, double  *covar)
{
    (*covar) = 0.;
    if (deriv==0) return;
    --deriv;
    int lim, ic, jc;
    lim = ((*ntrk) + 1) * 3;
    for (ic = 1; ic <= lim; ++ic) {
	for (jc = 1; jc <= lim; ++jc) {
	    (*covar) += deriv[ic] * ader_ref(ic, jc) * deriv[jc];
	}
    }
} 
#undef ader_ref
*/


void cfmasserrold_(long int *ntrk, long int *list, double *parfs,
        double *ams, double *deriv, double BMAG, double *dm, double *sigm)
{
    int  i__;
    double ptot[4];
    double constB, dm2dpx, dm2dpy, dm2dpz, ee, pt, px, py, pz, cth;

    --deriv;
    --ams;
    parfs -= 4;
    --list;

    /* Function Body */
    ptot[0] = 0.;
    ptot[1] = 0.;
    ptot[2] = 0.;
    ptot[3] = 0.;

    constB = BMAG * vkalMagCnvCst ;

    int i3;
    for (i__ = 1; i__ <= (*ntrk); ++i__) {
	if (list[i__] == 1) {
	    i3 = i__ * 3;
	    pt = fabs(parfs[i3 + 3]);
	    pt = constB / pt;
	    cth = 1. /tan(parfs[i3 + 1]);
	    px = pt * cos(parfs[i3 + 2]);
	    py = pt * sin(parfs[i3 + 2]);
	    pz = pt * cth;
	    ptot[0] += px;
	    ptot[1] += py;
	    ptot[2] += pz;
	    ptot[3] += sqrt(px*px + py*py + pz*pz + ams[i__]*ams[i__]);
	}
    }


    for (i__ = 1; i__ <= (*ntrk); ++i__) {
	i3 = i__ * 3;
	if (list[i__] == 1) {
	    pt = fabs(parfs[i3+3]);
	    pt = constB / pt;
	    cth = 1. / tan(parfs[i3+1]);
	    px  = pt * cos(parfs[i3+2]);
	    py  = pt * sin(parfs[i3+2]);
	    pz  = pt * cth;
	    ee = sqrt(px*px + py*py + pz*pz + ams[i__]*ams[i__]);
	    dm2dpx = (ptot[3] / ee * px - ptot[0]) * 2.;
	    dm2dpy = (ptot[3] / ee * py - ptot[1]) * 2.;
	    dm2dpz = (ptot[3] / ee * pz - ptot[2]) * 2.;
	    deriv[i3 + 1] = dm2dpz * ((-pt) * (cth * cth + 1.));                    /* d(M2)/d(Theta) */
	    deriv[i3 + 2] =  -dm2dpx * py + dm2dpy * px;                            /* d(M2)/d(Phi)   */
	    deriv[i3 + 3] = (-dm2dpx * px - dm2dpy*py - dm2dpz*pz)/ parfs[i3+3];    /* d(M2)/d(Rho) */
/* cc Std derivatives-------------------------------------------*/
/*          DCV(6,I*3+1)=-PT*(1+CTH**2)           ! dPz/d(theta)*/
/*          DCV(4,I*3+2)=-PY                      ! dPx/d(phi)  */
/*          DCV(5,I*3+2)= PX                      ! dPy/d(phi)  */
/*          DCV(4,I*3+3)=-PX/PARFS(3,I)           ! dPx/d(rho)  */
/*          DCV(5,I*3+3)=-PY/PARFS(3,I)           ! dPy/d(rho)  */
/*          DCV(6,I*3+3)=-PZ/PARFS(3,I)           ! dPz/d(rho)  */
/* d(M2)/d(Rho) */
	} else {
	    deriv[i3 + 1] = 0.;
	    deriv[i3 + 2] = 0.;
	    deriv[i3 + 3] = 0.;
	}
    }
    deriv[1] = 0.;
    deriv[2] = 0.;
    deriv[3] = 0.;
    double covarm2=ptot[3]; //To avoid compiler warning
    //cferrany_(ntrk, &deriv[1], &covarm2);
    (*dm) = (ptot[3]-ptot[2])*(ptot[3]+ptot[2])-ptot[1]*ptot[1]-ptot[0]*ptot[0];
    if((*dm)<1.e-6)  (*dm) = 1.e-6;
    (*dm)   = sqrt(*dm);
    (*sigm) = sqrt(covarm2) / 2. / (*dm);
} 



} /* End of namespace */

