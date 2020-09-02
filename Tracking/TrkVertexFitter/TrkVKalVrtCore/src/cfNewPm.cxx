/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/Propagator.h"
#include "TrkVKalVrtCore/TrkVKalUtils.h"
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include <cmath>
#include <iostream>

namespace Trk {

extern const vkalMagFld  myMagFld;


extern double d_sign(double, double);
extern void cfnewp(long int *, double *, double *, double *, double *, double *);
extern void vkgrkuta_(double *, double *, double *, double *, const VKalVrtControlBase* =nullptr );


void cfnewpm(double *par, double *xyzStart, double *xyzEnd, double *ustep, double *parn, double *closePoint, const VKalVrtControlBase * CONTROL)
{
    double d__1, d__2,dist_left;
    double vect[7], stmg, vout[7]={0.}, dpar0[5];
    double perig[3], dstep, xyzst[3], charge;
    double posold, poscur, totway, dp;
    double dX, dY, dZ;
    long int ich;

/* --------------------------------------------------------- */
/*  The same as CFNEWP but for the case on nonuniform        */
/*   magnetic field                                          */
/*                                                           */
/*  Propagation from xyzStart reference point to xyzEnd point*/
/*     PAR  - perigee parameters wrt xyzStart                */
/*     PARN - perigee parameters wrt xyzEnd                  */
/*  Author: V.Kostyukhin                                     */
/* --------------------------------------------------------- */
    /* Parameter adjustments */
    --par;

    d__1 = tan(par[3]);
    totway = (*ustep) * sqrt(1. / (d__1 * d__1) + 1.);

    if (fabs(*ustep) < 10. && fabs(totway) < 20.)  return;   // Distance(mm) is small. Simplest propagation is used

    stmg = 40.;  //Propagation step in mm for nonuniform field

    vect[0] =  sin(par[4]) * par[1]   +xyzStart[0];
    vect[1] = -cos(par[4]) * par[1]   +xyzStart[1];
    vect[2] = par[2]                  +xyzStart[2];

    double constB = myMagFld.getMagFld(vect,CONTROL) * myMagFld.getCnvCst();

    double pt = constB / fabs(par[5]);
    double px = pt * cos(par[4]);
    double py = pt * sin(par[4]);
    double pz = pt / tan(par[3]);
    double p = sqrt(pt * pt + pz * pz);

    vect[3] = px / p;
    vect[4] = py / p;
    vect[5] = pz / p;
    vect[6] = p;
    charge = -d_sign( 1., par[5]);
    poscur = 0.;
//std::cout <<"VkGrkuta vect="<<vect[0]<<", "<<vect[1]<<", "<<vect[2]<<", "<<vect[3]<<", "
//                            <<vect[4]<<", "<<vect[5]<<", "<<vect[6]<<'\n';
    while(fabs(poscur) < fabs(totway)) {
	posold = poscur;
	d__1 = fabs(poscur) + stmg;
        d__2 = fabs(totway);
	poscur = d__1 < d__2 ? d__1 : d__2;
	poscur = d_sign(poscur, totway);
	dstep = poscur - posold;
	vkgrkuta_(&charge, &dstep, vect, vout, CONTROL);
	vect[0] = vout[0];
	vect[1] = vout[1];
	vect[2] = vout[2];
	vect[3] = vout[3];
	vect[4] = vout[4];
	vect[5] = vout[5];
// safety for strongly nonuniform field
	dX = vect[0]-xyzEnd[0];
	dY = vect[1]-xyzEnd[1];
	dZ = vect[2]-xyzEnd[2];
	dist_left = sqrt( dX*dX + dY*dY + dZ*dZ);
	if(dist_left < stmg) break;    // arrived
    }

/* --- Now we are in the new point. Calculate track parameters */
/*   at new point with new mag.field */

    constB = myMagFld.getMagFld(xyzEnd,CONTROL) * myMagFld.getCnvCst() ;

    dpar0[0] = 0.;
    dpar0[1] = 0.;
    dpar0[2] = acos(vout[5]);
    dpar0[3] = atan2(vout[4], vout[3]);
//    if (dpar0[3] < 0.) dpar0[3] += 6.2831853071794;

    px = vect[3] * vect[6];
    py = vect[4] * vect[6];
    dpar0[4] = d_sign(  (constB/sqrt(px*px+py*py)), par[5]);    /* new value of curvature */
    ich = (long int) charge;
    xyzst[0] = xyzEnd[0] - vout[0];
    xyzst[1] = xyzEnd[1] - vout[1];
    xyzst[2] = xyzEnd[2] - vout[2];

    cfnewp(&ich, dpar0, xyzst, &dp, parn, perig); // Last step of propagation 
    closePoint[0] = perig[0] + vout[0];                  // with simple program
    closePoint[1] = perig[1] + vout[1];
    closePoint[2] = perig[2] + vout[2];
} 




} /* End of namespace */
