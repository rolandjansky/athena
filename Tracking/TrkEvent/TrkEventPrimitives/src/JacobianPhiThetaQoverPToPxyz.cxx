/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPhiThetaQoverPToPxyz.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianPhiThetaQoverPToPxyz.h"
#include <cmath>

Trk::JacobianPhiThetaQoverPToPxyz::JacobianPhiThetaQoverPToPxyz(const double phi, const double theta, const double qOverP):
    AmgMatrix(3,3)()
{
    double p = 1/std::fabs(qOverP);
    double sign = - std::fabs(qOverP)/qOverP;
    (*this)(0,0) = -std::sin(phi)*std::sin(theta)/std::fabs(qOverP);
    (*this)(0,1) = std::cos(phi)*std::cos(theta)/std::fabs(qOverP);
    (*this)(0,2) = sign*std::pow(p,2)*std::cos(phi)*std::sin(theta);
    (*this)(1,0) = std::cos(phi)*std::sin(theta)/std::fabs(qOverP);
    (*this)(1,1) = std::sin(phi)*std::cos(theta)/std::fabs(qOverP);
    (*this)(1,2) = sign*std::pow(p,2)*std::sin(phi)*std::sin(theta);
    (*this)(2,0) = 0.;
    (*this)(2,1) = -std::sin(theta)/std::fabs(qOverP);
    (*this)(2,2) = sign*std::pow(p,2)*std::cos(theta);

}

