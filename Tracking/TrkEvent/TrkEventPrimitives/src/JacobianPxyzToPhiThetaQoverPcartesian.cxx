/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPxyzToPhiThetaQoverPcartesian.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianPxyzToPhiThetaQoverPcartesian.h"
#include <cmath>

Trk::JacobianPxyzToPhiThetaQoverPcartesian::JacobianPxyzToPhiThetaQoverPcartesian(const double px, const double py, const double pz, double charge):
    AmgMatrix(3,3)()
{	
    // initialize to zero
    this->setZero();

    double r = std::pow(px,2) + std::pow(py,2) + std::pow(pz,2);
    double l = std::pow(px,2) + std::pow(py,2);
    (*this)(0,0) = -py/l;
    (*this)(0,1) = px/l;
    (*this)(0,2) = 0.;
    (*this)(1,0) = px*pz/(r*std::sqrt(l));
    (*this)(1,1) = py*pz/(r*std::sqrt(l));
    (*this)(1,2) = -std::sqrt(l)/r;
    (*this)(2,0) = -charge*px/std::pow(std::sqrt(r),3);
    (*this)(2,1) = -charge*py/std::pow(std::sqrt(r),3);
    (*this)(2,2) = -charge*pz/std::pow(std::sqrt(r),3);

}

