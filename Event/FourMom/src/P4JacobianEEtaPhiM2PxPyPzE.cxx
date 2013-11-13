/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P4JacobianEEtaPhiM2PxPyPzE.h"

#include <cmath>
#include <algorithm>

P4JacobianEEtaPhiM2PxPyPzE::P4JacobianEEtaPhiM2PxPyPzE( double E, 
							double eta, 
							double phi, 
							double M) :
  HepMatrix(4,4,0)
{
  double t = exp(-eta); // == tg(theta/2)
  double P = std::sqrt( std::max( 0., E*E - M*M));
  double u = 1+t*t; 
  double cosTheta = (1-t*t)/u;
  double sinTheta = 2*t/u;
  double sinPhi = sin(phi);
  double cosPhi = cos(phi);
  double pt = P * sinTheta; 

  (*this)[0][0] = E/P * sinTheta*cosPhi; // dpx/dE
  (*this)[1][0] = E/P * sinTheta*sinPhi; // dpy/dE
  (*this)[2][0] = E/P * cosTheta;        // dpz/dE
  (*this)[3][0] = 1;                     // dE/dE

  (*this)[0][1] = -P * cosPhi * cosTheta*sinTheta; // dpx/deta
  (*this)[1][1] = -P * sinPhi * cosTheta*sinTheta; // dpy/deta
  (*this)[2][1] = P * sinTheta*sinTheta;           // dpz/deta;

  (*this)[0][2] = -pt * sinPhi;  // dpx/dphi
  (*this)[1][2] = pt * cosPhi;   // dpy/dphi

  (*this)[0][3] = -M/P * sinTheta*cosPhi; // dpx/dM
  (*this)[1][3] = -M/P * sinTheta*sinPhi; // dpy/dM
  (*this)[2][3] = -M/P * cosTheta;        // dpz/dM
}
