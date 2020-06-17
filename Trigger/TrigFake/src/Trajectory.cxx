/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<cmath>
#include "Trajectory.h"

namespace TrigFake{
Trajectory::Trajectory(double px_MeV, double py_MeV, double pz_MeV, 
		       double vx_mm, double vy_mm, double vz_mm, double q) {

  double px = px_MeV/1000; // GeV
  double py = py_MeV/1000; // GeV
  double pz = pz_MeV/1000; // GeV
		       
  double vx = vx_mm/10; // cm
  double vy = vy_mm/10; // cm
  double vz = vz_mm/10; // cm

  m_q = q;
  m_pz = pz; 

  m_pT = sqrt( px*px + py*py ); 
  m_theta = atan2(m_pT,m_pz);
  
  double pzpt=m_pz/m_pT;

  m_eta=log(pzpt+sqrt(1.0+pzpt*pzpt));

  double Rv  = sqrt( vx*vx + vy*vy ); 

  // calculate parameters at closest approach to origin
  
#define Bfield 2.
  m_Rcurv = 100. * m_pT/(0.3*Bfield);  // cm; 
  
  if (m_q != 0) {
    double phiR = atan2(py,px) - m_q*M_PI_2;
    double Ox = vx + m_Rcurv * cos(phiR);
    double Oy = vy + m_Rcurv * sin(phiR);
    double Ro = sqrt(Ox*Ox + Oy*Oy);
    double d01 = Ro + m_Rcurv;
    double d02 = Ro - m_Rcurv;
    if (d01*d02 > 0) {
      m_d0 = std::min(fabs(d01), fabs(d02));
    } else {
      m_d0 = -std::min(fabs(d01), fabs(d02));
    }
    m_phi0 = atan2(Oy,Ox) + m_q*M_PI_2;
    m_z0 = vz - m_pz * (Rv - fabs(m_d0)) / m_pT;
  } else {
    m_phi0 = atan2(py,px);
    m_d0 = fabs(vx * sin(m_phi0) - vy * cos(m_phi0));
    m_z0 = vz - m_pz * (Rv - m_d0) / m_pT;
  }

  while (m_phi0 < -M_PI) m_phi0 = m_phi0 + 2.*M_PI;
  while (m_phi0 > M_PI) m_phi0 = m_phi0 - 2.*M_PI;

}


/* ---------------------------------------------------------------------- */
std::pair<double,double> Trajectory::etaPhiAtCylinder(double rC_mm, double zC_mm) const {
    
  double rC = rC_mm/10.; //cm
  double zC = zC_mm/10.; //cm 

  // Calculate intercept with cylinder
  
  double phiC = 0;
  // double etaC = 0; // etaC is not being used -> compiler warnings
  if (rC > 0  && fabs(zC) > 0 && m_pT != 0 && fabs(m_z0) < zC) {

    /* r, z of intersection with cylinder */
    
    double r;
    double z = m_z0 + m_pz * (rC - fabs(m_d0)) / m_pT;
    if (fabs(z) > fabs(zC)) {
      /* track passes through the end of the cylinder */
      if (z > 0) z = fabs(zC); else z = -fabs(zC); 
      r = fabs(m_d0) + (z - m_z0) * m_pT / m_pz;
      r = fabs(r);   /* NOT SURE IF -ve r is handled correctly */
    } else r = rC;      /* hits barrel of cylinder */
    
    /* etaC is not being used -> compiler warnings
       double theta = atan2(r, fabs(z)) / 2.;

       if (z > 0) etaC = -log(tan(theta));
       else etaC = log(tan(theta));
    */

    /* Now calculate phiC */

    if (m_q == 0) {                /* Neutral Track */
      if (fabs(m_d0) <= r) phiC = m_phi0 - asin(fabs(m_d0)/r);
    } else {                      /* Charged track */
      double xD = 2. * r*(m_Rcurv + m_d0);
      if (xD != 0) {
	double xN = m_d0 * m_d0 + r * r + 2. * m_Rcurv * m_d0;
	if (fabs(xN)/fabs(xD) > 1) {
	  /* trajectory does not reach r, track loops
	  ***temporary set dphi to 0 for this case */
	  phiC = 2. * M_PI;
	} else {
	  double dphi = m_q*acos(xN/xD);
	  /* two solutions from the 2 intersections of the 2 circles,
	     but phi0 defines a direction, so the first solution
	     should be the one we want....*/
	  phiC = m_phi0 - m_q*M_PI_2 + dphi;
	}
      }   else phiC = 0; /* means track is orbiting the origin */
      
    }
    /* fold back into 0-2pi */
    while (phiC > M_PI) phiC = phiC - 2. * M_PI;
    while (phiC < -M_PI) phiC = phiC + 2. * M_PI;
  }

  std::pair<double, double> etaPhi(m_eta, phiC);
  return etaPhi;
}

}
