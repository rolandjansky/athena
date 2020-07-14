/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/TrkVKalUtils.h"
#include <cmath>
#include <iostream>

namespace Trk {


void cferpr(long int *ich, double  *par, double  *ref, double  *s0, double  *errold, double  *errnew)
{


    double  r__, dsphi, dseps, dsrho, cs, sn, xp, yp;
    double  derivm[25];	/* was [5][5] */
    double  ctg, dsq, dyp, d__3;

    extern void tdasatVK(double  *, double  *, double  *, long int , long int );


/*     ------------------------------------------ */
/*       This routine propagates the trajectory error matrix */
/*       originally evaluated at s=0 to the curved abcissa S */
/*       assuming that the track travels into VACUUM */
/*        INPUT:        ICH    Charge of track */
/* 		       PAR(5) Track parameters */
/* 		       REF(3) New reference point */
/*                      ERROLD error matrix at (0,0,0) */
/*                      S0     curved abcissa of propagation */
/*        OUTPUT:    ERRNEW    New error matrix */
/* ------ */
/*  derivm(i,j)=D(X'.j)/D(X.i) */
/* ----------------------------------------------------------- */
/* Author: V.Kostyukhin */
/* ------------------------------------------------------------ */
    /* Parameter adjustments */
    --par;
    --ref;


 /* --------------------------------------------------- */
    for (int ii= 0; ii< 25; ++ii)  derivm[ii] = 0.;
/* -- */
    cs = cos(par[4]);
    sn = sin(par[4]);
    xp = ref[1] * cs + ref[2] * sn; //==(*s0) for neutral particles
    yp = ref[1] * sn - ref[2] * cs;
    double sinp3 = sin(par[3]);
    ctg = cos(par[3]) / sinp3;
    derivm[6] = 1.;
    derivm[12] = 1.;
    derivm[24] = 1.;
    if (*ich != 0) {
	r__ = 1. / par[5];
	dyp = r__ - par[1] + yp;
	dsphi = -r__ * (yp * dyp + xp * xp) / (dyp * dyp + xp * xp);
	dseps =  r__ * xp / (dyp * dyp + xp * xp);
	dsrho = -r__ * (*s0) + r__ * r__ * dseps;
	derivm[5] = dseps * ctg;
	derivm[7] = -(*s0) / (sinp3 * sinp3);
	derivm[8] = dsphi * ctg;
	derivm[9] = dsrho * ctg;
	derivm[15] = par[5] * dseps;
/* Computing 2nd power */
	d__3 = r__ - par[1];
	dsq = sqrt(ref[1]*ref[1] + ref[2]*ref[2] + d__3*d__3 + 2.*d__3*yp);
	derivm[4] = -(r__*r__) + d_sign(1., r__) * (r__*r__) * dyp / dsq;
	derivm[3] = d_sign(1., r__) * (par[1] - r__) * xp / dsq;
	derivm[19] = *s0 + par[5] * dsrho;
	derivm[0] = d_sign(1., r__) * dyp / dsq;
	derivm[18] = par[5] * dsphi + 1.;
    } else {
	derivm[0] = 1.;
	derivm[18] = 1.;
      //derivm[3] = par[1] - xp;   VK Error!!!
	derivm[3] =  - xp;                  //dEps/dPhi
	derivm[7] = -xp / (sinp3 * sinp3);  //dZ/dTheta
	derivm[8] = -yp * ctg;              //dZ/dPhi
    }
    tdasatVK(derivm, &errold[0], &errnew[0], 5, 5);
   
} 


}  /* End of VKalVrtCore namespace */

