/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOMUTILS_THRUST_H
#define FOURMOMUTILS_THRUST_H

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

namespace FourMomUtils
{

  typedef INavigable4MomentumCollection::const_iterator I4MomIter_t;

  CLHEP::Hep3Vector thrust( const I4MomIter_t& iBeg, const I4MomIter_t& iEnd,
                            double& thrust_major, double& thrust_minor,
                            bool useThreeD=false );

  inline
  CLHEP::Hep3Vector thrust( const INavigable4MomentumCollection* theParticles,
                            double& thrust_major, double& thrust_minor,
                            bool useThreeD=false ) {
    return thrust( theParticles->begin(), theParticles->end(),
                   thrust_major, thrust_minor,
                   useThreeD );
  }
}

#endif

#endif // FOURMOMUTILS_THRUST_H
