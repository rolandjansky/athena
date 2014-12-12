/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_P4_TRK_H
#define TRACK_P4_TRK_H

//-----------------------------------------------------------------------------
//
// file:   Track_p1.h
//
//-----------------------------------------------------------------------------

#include <vector>

namespace Trk
{
  class Track_p4
  {
  public:
    Track_p4() : m_fitter(0), m_particleHypo(0), m_properties(0), m_patternRecognition(0), m_extPatternRecognition(0), m_chiSquared(0.0), m_numberDoF(0.0) {}

    //copied from trackInfo_p1
    unsigned int m_fitter;
    unsigned int m_particleHypo;
    unsigned int m_properties;
    unsigned int m_patternRecognition;
    unsigned int m_extPatternRecognition; //!< overflow if bitset>32 bits
    
    // copied from FitQuality_p1
    float  m_chiSquared;
    float  m_numberDoF;

    std::vector< TPObjRef >  m_trackState;
  };
}

#endif // TRACK_P4_TRK_H

