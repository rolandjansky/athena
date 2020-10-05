/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

 ///////////////////////////////////////////////////////////////////
// JacobianCartesianToPolar.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianCartesianToPolar.h"
#include <cmath>

Trk::JacobianCartesianToPolar::JacobianCartesianToPolar(const double lx, const double ly):
    AmgMatrix(5,5)()
{
     // initialize to identify matrix
     this->setIdentity();
    
     if (std::fabs(lx)>10e-6 || std::fabs(ly)>10e-6){
     
     double r = std::sqrt(lx*lx+ly*ly);
     
       (*this)(Trk::locR,Trk::locX)       = lx/r;
       (*this)(Trk::locR,Trk::locY)       = ly/r;
       (*this)(Trk::locPhi,Trk::locX)     = -ly/(r*r);
       (*this)(Trk::locPhi,Trk::locY)     = lx/(r*r);
     }    
}

