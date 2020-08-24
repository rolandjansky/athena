/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/CommonPars.h"
#include <cmath>
#include <iostream>

namespace Trk {

/* ================================================================== */
/*  Fast estimation of the crossing point of 2 charged tracks */
/*   p1(5),p2(5)  - perigee parameters of tracks */
/*   dBMAG         - magnetic field */
/*   out(3)       - vertex position estimation */
/* Author: V.Kostioukhine */
/* ================================================================== */

void vkvfast_(double *p1, double *p2, double *dbmag, double *out)
{
    void vkvFastV(double*, double*, double* vRef, double, double *out);
    vkvFastV(p1, p2, nullptr, (*dbmag), out);   }


void vkvFastV(double *p1, double *p2, double* vRef, double dbmag, double *out)
{
    extern void vkPerigeeToP( const double*, double*, double );
    double d__1;

    double diff, coef, cent1[2], cent2[2], angle, cross[6];/* was [3][2] */
    double r1, r2, z1, z2, a01[3], a02[3], dc, an[2];
    extern double vkvang_(double *, double *, double *, double *, double *);
    double ar1, ar2, xd1, xd2, yd1, yd2, dz1, dz2, pt1, pt2, pz1, pz2, det;
    double loc_bmag;


#define cross_ref(a_1,a_2) cross[(a_2)*3 + (a_1) - 4]

/* ---- */
    /* Parameter adjustments */
    --out;
    --p2;
    --p1;

    /* Function Body */
    loc_bmag = dbmag;
    if (dbmag <= .1) {loc_bmag = 0.1;}  // To avoid ZERO magnetic field 
/* ---- */
    double psum[3]={0.,0.,0.}; double ptmp[3]={0.,0.,0.};
    vkPerigeeToP(&p1[3],ptmp,loc_bmag); psum[0]+=ptmp[0];psum[1]+=ptmp[1];psum[2]+=ptmp[2];
    vkPerigeeToP(&p2[3],ptmp,loc_bmag); psum[0]+=ptmp[0];psum[1]+=ptmp[1];psum[2]+=ptmp[2];
/*------*/
    r1 = 1. / p1[5];
    r2 = 1. / p2[5];
    ar1 = fabs(r1);
    ar2 = fabs(r2);
//    pt1 = ar1 * loc_bmag * .00299979; // For GeV and CM
//    pt2 = ar2 * loc_bmag * .00299979;
//    pt1 = ar1 * loc_bmag * .299979;  // For MeV and MM
//    pt2 = ar2 * loc_bmag * .299979;
    pt1 = ar1 * loc_bmag * vkalMagCnvCst;  // from CommonPars.h
    pt2 = ar2 * loc_bmag * vkalMagCnvCst;
    pz1 = pt1 / tan(p1[3]);
    pz2 = pt2 / tan(p2[3]);
/* --- */
    xd1 = cos(p1[4]);
/* Need for calculation of Z of crossing point */
    xd2 = cos(p2[4]);
    yd1 = sin(p1[4]);
    yd2 = sin(p2[4]);
/* ---  Point of minimal approach to the beam */
    a01[0] =  p1[1] * yd1;
    a01[1] = -p1[1] * xd1;
    a01[2] =  p1[2];
    a02[0] =  p2[1] * yd2;
    a02[1] = -p2[1] * xd2;
    a02[2] =  p2[2];
/* -- coordinates of centres of trajectories */
    cent1[0] = a01[0] - r1 * yd1;
    cent1[1] = a01[1] + r1 * xd1;
    cent2[0] = a02[0] - r2 * yd2;
    cent2[1] = a02[1] + r2 * xd2;
/* -- */
    an[0] = cent2[0] - cent1[0];
    an[1] = cent2[1] - cent1[1];
    dc = sqrt(an[0]*an[0] + an[1]*an[1]);
    an[0] /= dc;
    an[1] /= dc;
/* -- Single point*/
    if (dc <= fabs(ar1 - ar2)) {
	if (ar1 < ar2) {
	    cross_ref(1, 1) = ar1 / dc * (cent1[0] - cent2[0]) + cent1[0];
	    cross_ref(1, 2) = ar2 / dc * (cent1[0] - cent2[0]) + cent2[0];
	    cross_ref(2, 1) = ar1 / dc * (cent1[1] - cent2[1]) + cent1[1];
	    cross_ref(2, 2) = ar2 / dc * (cent1[1] - cent2[1]) + cent2[1];
	} else {
	    cross_ref(1, 1) = ar1 / dc * (cent2[0] - cent1[0]) + cent1[0];
	    cross_ref(1, 2) = ar2 / dc * (cent2[0] - cent1[0]) + cent2[0];
	    cross_ref(2, 1) = ar1 / dc * (cent2[1] - cent1[1]) + cent1[1];
	    cross_ref(2, 2) = ar2 / dc * (cent2[1] - cent1[1]) + cent2[1];
	}
	angle = vkvang_(&cross_ref(1, 1), cent1, &r1, &xd1, &yd1);
	z1 = pz1 * r1 * angle / pt1 + a01[2];
	angle = vkvang_(&cross_ref(1, 2), cent2, &r2, &xd2, &yd2);
	z2 = pz2 * r2 * angle / pt2 + a02[2];
	//diffz = fabs(z2 - z1);
	out[1] = (cross_ref(1, 1) + cross_ref(1, 2)) / 2.;
	out[2] = (cross_ref(2, 1) + cross_ref(2, 2)) / 2.;
	out[3] = (z1 + z2) / 2.;
	return;
    }
/* ---------------------------------------------------------- */
    diff = dc - (ar1 + ar2);
    if (diff < 0.) {
/*  2 distinct points, A1 & A2. */
	coef = (r1*r1 + dc*dc - r2*r2) / (dc * 2);
	if (r1*r1 - coef*coef > 0.) {
	    det = sqrt(r1 * r1 - coef * coef);
	} else {
	    det = 0.;
	}
	cross_ref(1, 1) = cent1[0] + coef * an[0] + det * an[1];
	cross_ref(2, 1) = cent1[1] + coef * an[1] - det * an[0];
	cross_ref(3, 1) = 0.;
	cross_ref(1, 2) = cent1[0] + coef * an[0] - det * an[1];
	cross_ref(2, 2) = cent1[1] + coef * an[1] + det * an[0];
	cross_ref(3, 2) = 0.;
/* -First cross */
	angle = vkvang_(&cross_ref(1, 1), cent1, &r1, &xd1, &yd1);
	z1 = pz1 * r1 * angle / pt1 + a01[2];
	angle = vkvang_(&cross_ref(1, 1), cent2, &r2, &xd2, &yd2);
	z2 = pz2 * r2 * angle / pt2 + a02[2];
	dz1 = fabs(z2 - z1);
	cross_ref(3, 1) = (z1 + z2) / 2.;
/* -Second cross */
	angle = vkvang_(&cross_ref(1, 2), cent1, &r1, &xd1, &yd1);
	z1 = pz1 * r1 * angle / pt1 + a01[2];
	angle = vkvang_(&cross_ref(1, 2), cent2, &r2, &xd2, &yd2);
	z2 = pz2 * r2 * angle / pt2 + a02[2];
	dz2 = fabs(z2 - z1);
	cross_ref(3, 2) = (z1 + z2) / 2.;
/* if reterence vertex is present  -> check 2D mom direction -> must be avay from it*/
        if(vRef && fabs(dz1-dz2)<5.){
          double dir1=ptmp[0]*(cross_ref(1,1)-vRef[0])+ptmp[1]*(cross_ref(2,1)-vRef[1]);
          double dir2=ptmp[0]*(cross_ref(1,2)-vRef[0])+ptmp[1]*(cross_ref(2,2)-vRef[1]);
          if(dir1*dir2<0){   // points are on different sides of ref. vertex
            if(dir1<0) dz1+=1000.;
            if(dir2<0) dz2+=1000.;
          }
        }
/* - choice of best dz */

	if (dz1 < dz2) {
	    out[1] = cross_ref(1, 1);
	    out[2] = cross_ref(2, 1);
	    out[3] = cross_ref(3, 1);
	} else {
	    out[1] = cross_ref(1, 2);
	    out[2] = cross_ref(2, 2);
	    out[3] = cross_ref(3, 2);
	}
    } else {
	cross_ref(1, 1) = (ar1 * cent2[0] + ar2 * cent1[0]) / (ar1 + ar2);
	cross_ref(2, 1) = (ar1 * cent2[1] + ar2 * cent1[1]) / (ar1 + ar2);
	out[1] = cross_ref(1, 1);
	out[2] = cross_ref(2, 1);
	out[3] = 0.;
	d__1 = r1 * dc / (ar1 + ar2);
	angle = vkvang_(cross, cent1, &d__1, &xd1, &yd1);
	z1 = pz1 * r1 * angle / pt1 + a01[2];
	d__1 = r2 * dc / (ar1 + ar2);
	angle = vkvang_(cross, cent2, &d__1, &xd2, &yd2);
	z2 = pz2 * r2 * angle / pt2 + a02[2];
	out[3] = (z1 + z2) / 2.;
	//diffz = fabs(z1 - z2);
    }
} 
#undef cross_ref


double vkvang_(double *crs, double *centr, double *r__, double *xd, double *yd)
{
    double ret_val, cosf, sinf, cosp, sinp;

    /* Parameter adjustments */
    --centr;
    --crs;

    /* Function Body */
    sinf = (crs[1] - centr[1]) / *r__;
    cosf = -(crs[2] - centr[2]) / *r__;
    sinp = *xd * sinf - *yd * cosf;
/* vector prod. */
    cosp = *xd * cosf + *yd * sinf;
/* scalar prod. */
    ret_val = atan2(sinp, cosp);
    return ret_val;
} 


} /* End of VKalVrtCore namespace */
