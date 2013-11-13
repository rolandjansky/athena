/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P5toP4JacobianPhiThetaEM2PxPyPzE.h"
#include <math.h>

P5toP4JacobianPhiThetaEM2PxPyPzE::P5toP4JacobianPhiThetaEM2PxPyPzE(const double phi0, 
								   const double theta, 
								   const double E, 
								   const double mass ):
  CLHEP::HepMatrix(4,5,0)
{
  double sinp = sin(phi0);
  double cosp = cos(phi0);
  double sint = sin(theta);
  double cost = cos(theta);
  double mom  = sqrt((E*E)-(mass*mass));

  // transformation of track parameters
  double px = mom*cosp*sint;
  double py = mom*sinp*sint;
  double pz = mom*cost;
  double EoverP = E/mom;
  

  (*this)[0][2] = -py; // dpx/dphi0
  (*this)[0][3] =  pz*cosp; // dpx/dtheta
  (*this)[0][4] =  EoverP*sint*cosp; // dpx/E
  
  (*this)[1][2] =  px; // dpy/dphi0
  (*this)[1][3] =  pz*sinp; // dpy/dtheta
  (*this)[1][4] =  EoverP*sint*sinp; // dpy/dE
  
  (*this)[2][3] = -sint*mom; // dpz/dtheta
  (*this)[2][4] =  EoverP*cost; // dpz/dE
 
  (*this)[3][4] =  1.; // dE/dE
  
}

