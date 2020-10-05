/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPerigeeToCartesian.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianPerigeeToCartesian.h"
#include <cmath>

Trk::JacobianPerigeeToCartesian::JacobianPerigeeToCartesian(const double d0, 
                                                            const double,
                                                            const double phi0, 
                                                            const double theta, 
                                                            const double qOverP, 
                                                            const double mass ):
  AmgMatrix(7,5)()
{

  // initialize to zero
  this->setZero();

  double sinp = std::sin(phi0);
  double cosp = std::cos(phi0);
  double sint = std::sin(theta);
  double cost = std::cos(theta);
  double rho  = std::fabs(qOverP);
  int charge=+1;
  if(qOverP<0) { charge = -1;}

  // transformation of track parameters
  double px = cosp*sint/rho;
  double py = sinp*sint/rho;
  double pz = cost/rho;
  double E = std::sqrt(1/rho/rho+mass*mass);
  
  // I don't need this
  //double x = xref[0] - d0*sinp;
  //double y = xref[1] + d0*cosp;
  //double z = xref[2] + z0;

  (*this)(0,2) = -py; // dpx/dphi0
  (*this)(0,3) =  pz*cosp; // dpx/dtheta
  (*this)(0,4) = -charge*px/rho; // dpx/dqOverP  
  (*this)(1,2) =  px; // dpy/dphi0
  (*this)(1,3) =  pz*sinp; // dpy/dtheta
  (*this)(1,4) = -charge*py/rho; // dpy/dqOverP  
  (*this)(2,3) = -sint/rho; // dpz/dtheta
  (*this)(2,4) = -charge*pz/rho; // dpz/dqOverP  
  (*this)(3,4) = -charge*1./(E*rho*rho*rho); // dE/dqOverP  
  (*this)(4,0) = -sinp; // dx/dd0
  (*this)(4,2) = -d0*cosp; // dx/dphi0  
  (*this)(5,0) = +cosp; // dy/dd0
  (*this)(5,2) = -d0*sinp; // dy/dphi0
  (*this)(6,1) = 1.; //dz/dz0

}

