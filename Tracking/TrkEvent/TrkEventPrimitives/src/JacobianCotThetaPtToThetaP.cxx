/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianCotThetaPtToThetaP, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianCotThetaPtToThetaP.h"
#include <cmath>

Trk::JacobianCotThetaPtToThetaP::JacobianCotThetaPtToThetaP(const double cotTheta, const double qpT):
  AmgMatrix(5,5)()
{
    
   // initialize to identify matrix  
   this->setIdentity();
        
   double onePlusCotTSq = 1 + cotTheta*cotTheta;
   (*this)(3,3) = - 1./onePlusCotTSq;
   (*this)(4,3) = -cotTheta/(onePlusCotTSq*std::sqrt(onePlusCotTSq)) * qpT;
   (*this)(4,4) = 1./std::sqrt(onePlusCotTSq);
}





