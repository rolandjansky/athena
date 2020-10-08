/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOMUTILS_JETBROADENING_H
#define FOURMOMUTILS_JETBROADENING_H

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

namespace FourMomUtils
{
  typedef INavigable4MomentumCollection::const_iterator I4MomIter_t;

  bool jetBroadening(const I4MomIter_t& iBeg, const I4MomIter_t& iEnd,
                     double& wideJetBroadening, double& totalJetBroadening,
                     CLHEP::Hep3Vector thrust, bool useThreeD = false );

  inline
  bool jetBroadening(const INavigable4MomentumCollection* theParticles,
                     double& wideJetBroadening, double& totalJetBroadening,
                     CLHEP::Hep3Vector thrust, bool useThreeD = false ) {
    return jetBroadening( theParticles->begin(), theParticles->end(),
        wideJetBroadening, totalJetBroadening,
        thrust, useThreeD );
  }
}

#endif

#endif // FOURMOMUTILS_JETBROADENING_H
