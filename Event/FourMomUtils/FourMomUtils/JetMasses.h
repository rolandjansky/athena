/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOMUTILS_JETMASSES_H
#define FOURMOMUTILS_JETMASSES_H

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

namespace FourMomUtils
{
  typedef INavigable4MomentumCollection::const_iterator I4MomIter_t;

  bool jetMasses(const I4MomIter_t& iBeg, const I4MomIter_t& iEnd,
                 double& heavyJetMass, double& lightJetMass,
                 CLHEP::Hep3Vector thrust );

  inline
  bool jetMasses(const INavigable4MomentumCollection* theParticles,
                 double& heavyJetMass, double& lightJetMass,
                 CLHEP::Hep3Vector thrust ) {
    return jetMasses( theParticles->begin(), theParticles->end(),
                      heavyJetMass, lightJetMass, thrust );
  }
}

#endif

#endif // FOURMOMUTILS_JETMASSES_H
