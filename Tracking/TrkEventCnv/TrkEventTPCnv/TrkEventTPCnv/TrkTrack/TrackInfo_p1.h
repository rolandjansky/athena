/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKINFO_P1_TRK_H
#define TRACKINFO_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackInfo_p1.h
//
//-----------------------------------------------------------------------------

#include <vector>
//#include <bitset>


namespace Trk
{
   class TrackInfo_p1
   {
  public:
     TrackInfo_p1() : m_fitter(0), m_particleHypo(0), m_properties(0), m_patternRecognition(0), m_extPatternRecognition(0){}

     unsigned int m_fitter;
     unsigned int m_particleHypo;

//bitsets
    unsigned int m_properties;
    unsigned int m_patternRecognition;
    unsigned int m_extPatternRecognition; // overflow if bitset>32 bits

   };
}

#endif // TRACKINFO_P1_TRK_H
