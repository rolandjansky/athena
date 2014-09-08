/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ESTIMATED_BREM_ON_TRACK_P1_TRK_H
#define ESTIMATED_BREM_ON_TRACK_P1_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
  /** @brief persistent representation of EstimatedBremOnTrack
      @author: Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
  */
  class EstimatedBremOnTrack_p1
  {
  public:
    EstimatedBremOnTrack_p1() : m_retainedEnFraction(0.0), m_sigmaRetEnFraction(0.0),m_sigmaQoverPsquared(0.0), m_direction(0)  {}
    TPObjRef m_mefBase; // MaterialEffectsBase.h base class
    float  m_retainedEnFraction;
    float  m_sigmaRetEnFraction;
    float  m_sigmaQoverPsquared;
    signed char m_direction;

  };
}

#endif
