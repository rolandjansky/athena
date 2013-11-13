/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P4JacobianPxPyPzM2PxPyPzE.h"

#include <cmath>
#include <algorithm>

P4JacobianPxPyPzM2PxPyPzE::P4JacobianPxPyPzM2PxPyPzE( double px, 
						      double py, 
						      double pz, 
						      double M) :
  HepMatrix(4,4,0)
{
  double pt2 = px*px + py*py;
  double p2 = pt2 + pz*pz;
  double E = std::sqrt( std::max( 0., M*M + p2));

  (*this)[0][0] = 1; // dpx/dpx
  (*this)[1][1] = 1; // dpy/dpy
  (*this)[2][2] = 1; // dpz/dpz

  (*this)[3][0] = px/E;     // dM/dpx
  (*this)[3][1] = py/E;     // dM/dpy
  (*this)[3][2] = pz/E;     // dM/dpz
  (*this)[3][3] = M/E;      // dM/dE
}
