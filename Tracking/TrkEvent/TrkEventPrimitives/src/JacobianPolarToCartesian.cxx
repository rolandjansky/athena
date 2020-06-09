/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPolarToCartesian.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianPolarToCartesian.h"
#include <cmath>

Trk::JacobianPolarToCartesian::JacobianPolarToCartesian(const double r, const double phi):
    AmgMatrix(5,5)()
{
      // initialize to zero
      this->setIdentity();

      (*this)(Trk::locX,Trk::locR)       = std::cos(phi);
      (*this)(Trk::locX,Trk::locPhi)     = -r*std::sin(phi);
      (*this)(Trk::locY,Trk::locR)       = std::sin(phi);
      (*this)(Trk::locY,Trk::locPhi)     = r*std::cos(phi);    
}

