/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/EigenP4JacobianEEtaPhiM2PtEtaPhiM.h"

#include <cmath>
#include <algorithm>

EigenP4JacobianEEtaPhiM2PtEtaPhiM::EigenP4JacobianEEtaPhiM2PtEtaPhiM( double E, 
								      double eta, 
								      double M) :
  AmgMatrix(4,4)()
{
  double t = std::exp(-eta);
  double P = std::sqrt( std::max( 0., E*E - M*M));
  double u = 1+t*t; 
  double cosTheta = (1-t*t)/u;
  double sinTheta = 2*t/u;

  this->setZero();
  (*this)(0,0) = E/P * sinTheta;          // dPt/dE
  (*this)(0,1) = -P * cosTheta*sinTheta;  // dPt/deta
  (*this)(0,3) = -M/P * sinTheta;         // dPt/dM

  (*this)(1,1) = 1;  // deta/deta
  (*this)(2,2) = 1;  // dphi/dphi
  (*this)(3,3) = 1;  // dM/dM
}
