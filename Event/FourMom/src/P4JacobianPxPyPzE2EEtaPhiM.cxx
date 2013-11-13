/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P4JacobianPxPyPzE2EEtaPhiM.h"

#include <cmath>
#include <algorithm>

P4JacobianPxPyPzE2EEtaPhiM::P4JacobianPxPyPzE2EEtaPhiM( double px, 
							double py, 
							double pz, 
							double E) :
  HepMatrix(4,4,0)
{
  double pt2 = px*px + py*py;
  double p2 = pt2 + pz*pz;
  double p = std::sqrt(p2);
  double M = std::sqrt( std::max( 0., E*E - p2));

  (*this)[0][3] = 1; // dE/dE

  (*this)[1][0] = -px*pz/(p*pt2);  // deta/dpx
  (*this)[1][1] = -py*pz/(p*pt2);  // deta/dpy
  (*this)[1][2] =  1/p;            // deta/dpz

  (*this)[2][0] = -py/pt2;   // dphi/dpx
  (*this)[2][1] =  px/pt2;   // dphi/dpy

  (*this)[3][0] = -px/M;     // dM/dpx
  (*this)[3][1] = -py/M;     // dM/dpy
  (*this)[3][2] = -pz/M;     // dM/dpz
  (*this)[3][3] = E/M;       // dM/dE
}
