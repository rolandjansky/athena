/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/EigenP4JacobianPxPyPzE2PxPyPzM.h"

#include <cmath>
#include <algorithm>

EigenP4JacobianPxPyPzE2PxPyPzM::EigenP4JacobianPxPyPzE2PxPyPzM( double px, 
								double py, 
								double pz, 
								double E) :
  AmgMatrix(4,4)()
{
  double pt2 = px*px + py*py;
  double p2 = pt2 + pz*pz;
  double M = std::sqrt( std::max( 0., E*E - p2));
  this->setZero();

  (*this)(0,0) = 1; // dpx/dpx
  (*this)(1,1) = 1; // dpy/dpy
  (*this)(2,2) = 1; // dpz/dpz

  (*this)(3,0) = -px/M;     // dM/dpx
  (*this)(3,1) = -py/M;     // dM/dpy
  (*this)(3,2) = -pz/M;     // dM/dpz
  (*this)(3,3) = E/M;       // dM/dE
}
