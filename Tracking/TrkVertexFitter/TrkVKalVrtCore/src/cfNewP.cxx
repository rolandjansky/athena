/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

namespace Trk {

void cfnewp(long int *ich, double *parold, double *ref, double *s, double *parnew, double *per)
{
 
    double dphi, coth, hper, zper, zeps, r__, cs, xc, yc, sn, sipart, eps;

/* ------------------------------------------------------------*/
/*       This routine propagates the trajectory parameters     */
/*       w.r.t to a new reference point.                       */
/*   RETURNED POSITION IS NOT A POSITION OF CLOSEST APPROACH   */
/*   BUT PERIGEE ASSUMIMG THAT REF IS 			       */
/*            A CENTER OF COORDINATE SYSTEM                    */                         
/*							       */
/*        INPUT:  ICH    Charge(-1,0,1)                        */
/* 		 PAROLD Initial parameters(EPS,H,THE,PHI,1./R) */
/*                REF    Position of new reference point       */
/*        OUTPUT: S Curved abcissa of new perigee              */
/*                PARNEW Parameters evaluated at new perigee   */
/*                PER    Position of new perigee               */
/* Author: V.Kostyukhin                                        */
/* ------------------------------------------------------------*/
    /* Parameter adjustments */
    --per;
    --parnew;
    --ref;
    --parold;

    eps = parold[1];
    sn = sin(parold[4]);
    cs = cos(parold[4]);
    coth = 1. / tan(parold[3]);
    zeps = parold[2];

/*  Neutral track */
    if ( (*ich) == 0) {
	hper = eps - ref[1] * sn + ref[2] * cs;
	dphi = 0.;
	(*s) = ref[1] * cs + ref[2] * sn;
	zper = zeps + coth * (*s) - ref[3];
    } else {
	r__ = 1. / parold[5];
	sipart = 1.;
	if (r__ < 0.) sipart = -1.;
/* -- Centre of the circle and perigee */
	xc = (eps - r__) * sn - ref[1];
	yc = -(eps - r__) * cs- ref[2];
	hper = r__ - sipart * sqrt(xc*xc + yc*yc);
/* -- Phi */
        double tmp_prec=  - eps - cs*ref[2] + sn*ref[1];
	dphi = atan2(sipart * (ref[1]*cs + ref[2]*sn), 
                     sipart * (r__ + tmp_prec) );
	(*s) = r__*dphi;
/* -- Z at VXOLD */
	zper = zeps + coth * (*s) - ref[3];
    }
    parnew[1] = hper;
    parnew[2] = zper;
    parnew[3] = parold[3];
    parnew[4] = parold[4] + dphi;

/* check 2*pi period in phi */
    while (parnew[4] > 6.2831853071794) parnew[4] -= 6.2831853071794;
    while (parnew[4] <-6.2831853071794) parnew[4] += 6.2831853071794;



    parnew[5] = parold[5];
    per[1] = ref[1] + sin(parold[4] + dphi) * hper;
    per[2] = ref[2] - cos(parold[4] + dphi) * hper;
    per[3] = ref[3] + zper;
} 


} /* End of VKalVrtCore namespace */
