/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianThetaPToCotThetaPt, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include <cmath>

Trk::JacobianThetaPToCotThetaPt::JacobianThetaPToCotThetaPt(const double theta, const double qp):
  AmgMatrix(5,5)()
{
   // initialize to zero
   this->setIdentity();

   double sintheta = std::sin(theta);
   (*this)(3,3) = - 1./(sintheta*sintheta);
   (*this)(4,3) = - qp * std::cos(theta)/(sintheta*sintheta);
   (*this)(4,4) = 1.0 / sintheta;
}
