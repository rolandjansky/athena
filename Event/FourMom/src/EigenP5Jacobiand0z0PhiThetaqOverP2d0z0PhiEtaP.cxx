/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/EigenP5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP.h"
#include "EventPrimitives/EventPrimitives.h"

#include <cmath>
#include <algorithm>

EigenP5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP::EigenP5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP(const double phi, const int charge, const double momentum) :
  AmgMatrix(5,5)()
{

  this->setIdentity();
  (*this)(0,0) = 1.;
  (*this)(1,1) = 1.;
  (*this)(2,2) = 1.;
  (*this)(3,3) = 1.;
  (*this)(4,4) = 1.;

  double sinp = std::sin(phi);
  
  (*this)(3,0) = 0.;                   // deta/dd0
  (*this)(3,1) = 0.;                   // deta/dz0
  (*this)(3,2) = 0.;                   // deta/dphi
  (*this)(3,3) =-1*(1./sinp);     // deta/dtheta
  (*this)(3,4) = 0.;                   // deta/d(q/p)

  (*this)(4,0) = 0.;                   // dp/dd0
  (*this)(4,1) = 0.;                   // dp/dz0
  (*this)(4,2) = 0.;                   // dp/dphi
  (*this)(4,3) = 0.;                   // dp/theeta
  (*this)(4,4) = -1*charge*(momentum*momentum);     // dp/d(q/p)

}
