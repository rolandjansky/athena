/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "TrigInDetEvent/TrigInDetTrackHelper.h"


/* ---------------------------------------------------------------------- */
void TrigInDetTrackHelper::extrapolate(double rC, double zC, double &phiC, double &etaC) const {
/*----------------------------------------------------------------------*

 *           pt    (I) : pt                                             *
 *           pz    (I) : pz                                             *
 *           q     (I) : charge                                         *
 *           phi0  (I) : phi of dirn vec at closest approach to (0,0)   *
 *           d0    (I) : dist. (mm) of close appr. to (0,0) in xy plane *
 *           z0    (I) : z at r=d0           (mm)                       *
 *           rC    (I) : radius of Cylinder  (mm)                       *
 *           zC    (I) : half-length in z of Cylinder   (mm)            *
 *           phiC  (O) : phi of track at (rC, zC)                       *
 *           etaC  (O) : eta of track at (rC, zC)                       *
 *                                                                      *
 * Note : looping tracks are not treated correctly, phiC is set twopi   *
 *                                                                      *
 * Author : John Baines                        Date : 12/11/97          *
 *          from an original concept by Simon George                    *
 *                                                                      *
 *----------------------------------------------------------------------*/
#define BFIELD 2.0
  double Rcurv,r, z, xD, xN, dphi; 

  double pt = m_param->pT();
  double eta = m_param->eta();
  double q = 1.;
  if (pt < 0) q = -1.; 
  double theta = 2.*atan(exp(-fabs(eta)));
  double pz =  fabs(pt) / tan(theta);
  if (eta < 0) pz = -pz;
  double d0 = m_param->a0();
  double z0 = m_param->z0();
  double phi0 = m_param->phi0();
 
  phiC = 0;
  etaC = 0;
  if (rC > 0  && fabs(zC) > 0 && pt != 0 && fabs(z0) < zC) {

    /* r, z of intersection with cylinder */
    
    z = z0 + pz * (rC - fabs(d0)) / fabs(pt);
    if (fabs(z) > fabs(zC)) {
      /* track passes through the end of the cylinder */
      if (z > 0) z = fabs(zC); else z = -fabs(zC); 
      r = fabs(d0) + (z - z0) * fabs(pt) / pz;
      r = fabs(r);   /* NOT SURE IF -ve r is handled correctly */
    } else r = rC;      /* hits barrel of cylinder */
    
    theta = atan2(r, fabs(z)) / 2.;
    if (z > 0) etaC = -log(tan(theta));
    else etaC = log(tan(theta));
    
    /* Now calculate phiC */

    if (q == 0) {                /* Neutral Track */
      if (fabs(d0) <= r) phiC = phi0 - asin(fabs(d0)/r);
    } else {                      /* Charged track */
      Rcurv = fabs(pt)/(0.3*BFIELD);
      xD = 2. * r*(Rcurv + d0);
      if (xD != 0) {
	xN = d0 * d0 + r * r + 2. * Rcurv * d0;
	if (fabs(xN)/fabs(xD) > 1) {
	  /* trajectory does not reach r, track loops
	  ***temporary set dphi to 0 for this case */
	  phiC = 2. * M_PI;
	} else {
	  dphi = q*acos(xN/xD);
	  /* two solutions from the 2 intersections of the 2 circles,
	     but phi0 defines a direction a direction, so the first solution
	     should be the one we want....*/
	  phiC = phi0 - q*M_PI_2 + dphi;
	}
      }   else phiC = 0; /* means track is orbiting the origin */
      
    }
    /* fold back into -pi to pi */
    while (phiC > M_PI) phiC = phiC - 2. * M_PI;
    while (phiC < -M_PI) phiC = phiC + 2. * M_PI;
  }
}
