/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include <cmath>
#include <iostream>

namespace Trk {

extern const vkalMagFld  myMagFld;

void vkgrkuta_(double *charge, double *step, double *vect, double *vout, const VKalVrtControlBase* CONTROL)
{
    double equiv_2[3], equiv_5[3];
    long int iter, ncut, j;
    double cost, pinv, rest, sint, a, b, c, f[4], hst;
    double hnorm, secxs[4], secys[4], seczs[4], f1, f2, h2, f3, h4, f4; 
    double g1, g2, g3, g4, g5, g6, at, bt, ct, ph, hp, fx, fy, fz, tl;
    double ph2, cba, rho, est, tet, hxp[3], dxt, dyt, dzt, ang2, rho1;



#define xyz (equiv_2)
#define x   (equiv_2)
#define y   (equiv_2 + 1)
#define z   (equiv_2 + 2)
#define xyzt (equiv_5)
#define xt   (equiv_5)
#define yt   (equiv_5 + 1)
#define zt   (equiv_5 + 2)


/* *******************************************************************/
/*                                                                   */
/*     Runge-Kutta method for tracking a particle through a magnetic */
/* .   field. Uses Nystroem algorithm (See Handbook Nat. Bur. of     */
/* .   Standards, procedure 25.5.20)                                 */
/* .                                                                 */
/* .     Input parameters                                            */
/* .           CHARGE    Particle charge                             */
/* .           STEP      Step size                                   */
/* .           VECT      Initial co-ords,direction cosines,momentum  */
/* .     Output parameters                                           */
/* .           VOUT      Output co-ords,direction cosines,momentum   */
/* .      User routine called                                        */
/*            CALL GUFLD(X,F)                                        */
/*           Authors    R.Brun, M.Hansroul  *********                */
/*                      V.Perevoztchikov (CUT STEP implementation)   */
/*      Taken from GEANT3.21                                         */
/*      Modified for VKalVrt   V.Kostyukhin                          */
/*********************************************************************/
/*             This constant is for units CM,GEV/C and KGAUSS */

    /* Parameter adjustments */
    --vout;
    --vect;

    iter = 0;
    ncut = 0;
    for (j = 1; j <= 7; ++j) {vout[j] = vect[j];}
    pinv = (*charge) * 2.9979251e-2 / vect[7];      // New for MM, MEV/C and KGAUSS
    tl = 0.;
    hst = *step;

//std::cout <<" Now in grkuta="<<vect[1]<<", "<<vect[2]<<", "<<vect[3]<<'\n';

L20:
    rest = *step - tl;
    if (fabs(hst) > fabs(rest)) hst = rest;
/* *****      CALL GUFLD(VOUT,F) */
    myMagFld.getMagFld( vout[1], vout[2], vout[3], fx, fy, fz, CONTROL);
    f[0] = fx*10.;   //VK Convert returned field in Tesla into kGauss for old code
    f[1] = fy*10.;
    f[2] = fz*10.;

/*  Start of integration */

    *x = vout[1];
    *y = vout[2];
    *z = vout[3];
    a = vout[4];
    b = vout[5];
    c = vout[6];

    h2 = hst * .5;
    h4 = h2  * .5;
    ph = pinv * hst;
    ph2 = ph * .5;
    secxs[0] = (b * f[2] - c * f[1]) * ph2;
    secys[0] = (c * f[0] - a * f[2]) * ph2;
    seczs[0] = (a * f[1] - b * f[0]) * ph2;
    ang2 = secxs[0]*secxs[0] + secys[0]*secys[0] + seczs[0]*seczs[0];
    if (ang2 > 9.86960440109)      goto L40;
    dxt = h2 * a + h4 * secxs[0];
    dyt = h2 * b + h4 * secys[0];
    dzt = h2 * c + h4 * seczs[0];
    *xt = *x + dxt;
    *yt = *y + dyt;
    *zt = *z + dzt;

/*  Second intermediate point */

    est = fabs(dxt) + fabs(dyt) + fabs(dzt);
    if (est > hst)	goto L30;
    
    
/* *****      CALL GUFLD(XYZT,F) */
    myMagFld.getMagFld( xyzt[0], xyzt[1], xyzt[2], fx, fy, fz, CONTROL);
    f[0] = fx*10.;   //VK Convert returned field in Tesla into kGauss for old code
    f[1] = fy*10.;
    f[2] = fz*10.;
    at = a + secxs[0];
    bt = b + secys[0];
    ct = c + seczs[0];

    secxs[1] = (bt * f[2] - ct * f[1]) * ph2;
    secys[1] = (ct * f[0] - at * f[2]) * ph2;
    seczs[1] = (at * f[1] - bt * f[0]) * ph2;
    at = a + secxs[1];
    bt = b + secys[1];
    ct = c + seczs[1];
    secxs[2] = (bt * f[2] - ct * f[1]) * ph2;
    secys[2] = (ct * f[0] - at * f[2]) * ph2;
    seczs[2] = (at * f[1] - bt * f[0]) * ph2;
    dxt = hst * (a + secxs[2]);
    dyt = hst * (b + secys[2]);
    dzt = hst * (c + seczs[2]);
    *xt = *x + dxt;
    *yt = *y + dyt;
    *zt = *z + dzt;
    at = a + secxs[2] * 2.;
    bt = b + secys[2] * 2.;
    ct = c + seczs[2] * 2.;

    est = fabs(dxt) + fabs(dyt) + fabs(dzt);
    if (est > fabs(hst) * 2.)          goto L30;
/* *****      CALL GUFLD(XYZT,F) */
    myMagFld.getMagFld( xyzt[0], xyzt[1], xyzt[2], fx, fy, fz, CONTROL);
    f[0] = fx*10.; //VK Convert returned field in Tesla into kGauss for old code
    f[1] = fy*10.;
    f[2] = fz*10.;

    *z   += (c + (seczs[0] + seczs[1] + seczs[2]) /3.) * hst;
    *y   += (b + (secys[0] + secys[1] + secys[2]) /3.) * hst;
    *x   += (a + (secxs[0] + secxs[1] + secxs[2]) /3.) * hst;

    secxs[3] = (bt * f[2] - ct * f[1]) * ph2;
    secys[3] = (ct * f[0] - at * f[2]) * ph2;
    seczs[3] = (at * f[1] - bt * f[0]) * ph2;
    a  += (secxs[0] + secxs[3] + (secxs[1] + secxs[2]) * 2.) /3.;
    b  += (secys[0] + secys[3] + (secys[1] + secys[2]) * 2.) /3.;
    c  += (seczs[0] + seczs[3] + (seczs[1] + seczs[2]) * 2.) /3.;

    est = fabs(secxs[0] + secxs[3] - (secxs[1] + secxs[2])) 
        + fabs(secys[0] + secys[3] - (secys[1] + secys[2])) 
	+ fabs(seczs[0] + seczs[3] - (seczs[1] + seczs[2]));

    if (est > 1e-4 && fabs(hst) > 1e-4)    goto L30;
    ++iter;
    ncut = 0;


/*  If too many iterations, go to HELIX */
    if (iter > 1992)  	goto L40;

    tl += hst;
    if (est < 3.125e-6)  
    hst *= 2.;
    cba = 1. / sqrt(a * a + b * b + c * c);
    vout[1] = *x;
    vout[2] = *y;
    vout[3] = *z;
    vout[4] = cba * a;
    vout[5] = cba * b;
    vout[6] = cba * c;
    rest = *step - tl;
    if (*step < 0.)  rest = -rest;
    if (rest > fabs(*step) * 1e-5)	goto L20;

    return;

/* *              CUT STEP */
L30:
    ++ncut;
/*               If too many cuts , go to HELIX */
    if (ncut > 11) {
	goto L40;
    }
    hst *= .5;
    goto L20;

/* *              ANGLE TOO BIG, USE HELIX */
L40:
    f1 = f[0];
    f2 = f[1];
    f3 = f[2];
    f4 = sqrt(f1*f1 + f2*f2 + f3*f3);
    rho = -f4 * pinv;
    tet = rho * *step;
    if (tet != 0.) {
	hnorm = 1. / f4;
	f1 *= hnorm;
	f2 *= hnorm;
	f3 *= hnorm;

	hxp[0] = f2 * vect[6] - f3 * vect[5];
	hxp[1] = f3 * vect[4] - f1 * vect[6];
	hxp[2] = f1 * vect[5] - f2 * vect[4];
	hp = f1 * vect[4] + f2 * vect[5] + f3 * vect[6];

	rho1 = 1. / rho;
	sint = sin(tet);
	cost = sin(tet/2.) * sin(tet/2.) * 2.;

	g1 = sint * rho1;
	g2 = cost * rho1;
	g3 = (tet - sint) * hp * rho1;
	g4 = -cost;
	g5 = sint;
	g6 = cost * hp;
	vout[1] = vect[1] + (g1 * vect[4] + g2 * hxp[0] + g3 * f1);
	vout[2] = vect[2] + (g1 * vect[5] + g2 * hxp[1] + g3 * f2);
	vout[3] = vect[3] + (g1 * vect[6] + g2 * hxp[2] + g3 * f3);
	vout[4] = vect[4] + (g4 * vect[4] + g5 * hxp[0] + g6 * f1);
	vout[5] = vect[5] + (g4 * vect[5] + g5 * hxp[1] + g6 * f2);
	vout[6] = vect[6] + (g4 * vect[6] + g5 * hxp[2] + g6 * f3);

    } else {
	vout[1] = vect[1] + *step * vect[4];
	vout[2] = vect[2] + *step * vect[5];
	vout[3] = vect[3] + *step * vect[6];

    }

    } 
#undef xyz
#undef zt
#undef yt
#undef xt
#undef z
#undef y
#undef x
#undef xyzt

} /* End of namespace */

