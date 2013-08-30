/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include "TrkVKalVrtCore/CommonPars.h"

namespace Trk {

extern VKalVrtBMag vkalvrtbmag;
extern void cferrany_(long int *, double *, double*);


void cfmasserr_(long int *ntrk, long int *list, double *parfs,
        double *ams, double *deriv, double *dm, double *sigm)
{
    long int  i__;
    double ptot[4];
    double const__, dm2dpx, dm2dpy, dm2dpz, ee, pt, px, py, pz, covarm2, cth;

    --deriv;
    --ams;
    parfs -= 4;
    --list;

    /* Function Body */
    ptot[0] = 0.;
    ptot[1] = 0.;
    ptot[2] = 0.;
    ptot[3] = 0.;

//VK    const__ = *locbmag * .0029979246;
    const__ = vkalvrtbmag.bmag * vkalMagCnvCst ;

    int i3;
    for (i__ = 1; i__ <= (*ntrk); ++i__) {
	if (list[i__] == 1) {
	    i3 = i__ * 3;
	    pt = fabs(parfs[i3 + 3]);
	    pt = const__ / pt;
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
	    pt = const__ / pt;
	    cth = 1. / tan(parfs[i3+1]);
	    px  = pt * cos(parfs[i3+2]);
	    py  = pt * sin(parfs[i3+2]);
	    pz  = pt * cth;
	    ee = sqrt(px*px + py*py + pz*pz + ams[i__]*ams[i__]);
	    dm2dpx = (ptot[3] / ee * px - ptot[0]) * 2.;
	    dm2dpy = (ptot[3] / ee * py - ptot[1]) * 2.;
	    dm2dpz = (ptot[3] / ee * pz - ptot[2]) * 2.;
	    deriv[i3 + 1] = dm2dpz * ((-pt) * (cth * cth + 1.));                    /* d(M2)/d(Theta) */
	    deriv[i3 + 2] =  -dm2dpx * py + dm2dpy * px;                          /* d(M2)/d(Phi)   */
	    deriv[i3 + 3] = (-dm2dpx * px - dm2dpy*py - dm2dpz*pz)/ parfs[i3+3];
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
    cferrany_(ntrk, &deriv[1], &covarm2);
    (*dm) = (ptot[3]-ptot[2])*(ptot[3]+ptot[2])-ptot[1]*ptot[1]-ptot[0]*ptot[0];
    if((*dm)<1.e-6)  (*dm) = 1.e-6;
    (*dm)   = sqrt(*dm);
    (*sigm) = sqrt(covarm2) / 2. / (*dm);
} 

} /* End of namespace */

