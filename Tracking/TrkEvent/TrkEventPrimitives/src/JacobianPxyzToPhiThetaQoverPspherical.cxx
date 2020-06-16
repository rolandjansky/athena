/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPxyzToPhiThetaQoverPspherical.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianPxyzToPhiThetaQoverPspherical.h"
#include <cmath>

Trk::JacobianPxyzToPhiThetaQoverPspherical::JacobianPxyzToPhiThetaQoverPspherical(const double phi, const double theta, const double qOverP):
    AmgMatrix(3,3)()
{	
    // initialize to zer
    this->setZero();

    double sinPhi = std::sin(phi);
    double cosPhi = std::cos(phi);
    double sinTheta = std::sin(theta);
    double cosTheta = std::cos(theta);
    (*this)(0,0)    = -std::fabs(qOverP)*sinPhi/sinTheta;
    (*this)(0,1)    = std::fabs(qOverP)*cosPhi/sinTheta;
    (*this)(0,2)    = 0.;
    (*this)(1,0)    = std::fabs(qOverP)*cosPhi*cosTheta;
    (*this)(1,1)    = std::fabs(qOverP)*sinPhi*cosTheta;
    (*this)(1,2)    = -std::fabs(qOverP)*sinTheta;
    (*this)(2,0)    = -std::pow(std::fabs(qOverP),3)*cosPhi*sinTheta/qOverP;
    (*this)(2,1)    = -std::pow(std::fabs(qOverP),3)*sinPhi*sinTheta/qOverP;
    (*this)(2,2)    = -std::pow(std::fabs(qOverP),3)*cosTheta/qOverP;

}

