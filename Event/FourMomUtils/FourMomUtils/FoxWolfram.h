/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOMUTILS_FOXWOLFRAM_H
#define FOURMOMUTILS_FOXWOLFRAM_H

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS

#include <vector>
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

namespace FourMomUtils
{

  typedef INavigable4MomentumCollection::const_iterator I4MomIter_t;

  bool foxWolfram( const I4MomIter_t& iBeg,
       const I4MomIter_t& iEnd,
                   std::vector<double>& H, unsigned int order = 5 );

  inline
  bool foxWolfram( const INavigable4MomentumCollection* theParticles,
                   std::vector<double>& H, unsigned int order = 5 ) {
    return foxWolfram( theParticles->begin(), theParticles->end(),
           H, order );
  }
}

#endif

#endif // FOURMOMUTILS_FOXWOLFRAM_H
