/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOMUTILS_FORWARDTERM_H
#define FOURMOMUTILS_FORWARDTERM_H

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS

#include "NavFourMom/INavigable4MomentumCollection.h"

namespace FourMomUtils
{

  typedef INavigable4MomentumCollection::const_iterator I4MomIter_t;

  double forwardTerm( const I4MomIter_t& iBeg,
          const I4MomIter_t& iEnd,
          double m_central,
          bool useThreeD = false );

  inline
  double forwardTerm( const INavigable4MomentumCollection* particles,
          double central, bool useThreeD = false ) {
    return forwardTerm( particles->begin(),
      particles->end(),
      central, useThreeD );
  }

}

#endif

#endif // FOURMOMUTILS_FORWARDTERM_H
