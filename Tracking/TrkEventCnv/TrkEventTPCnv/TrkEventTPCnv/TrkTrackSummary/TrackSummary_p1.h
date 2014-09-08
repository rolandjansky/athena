/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackSummary_p1.h
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------

#ifndef TRACK_SUMMARY_P1_TRK_H
#define TRACK_SUMMARY_P1_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class TrackSummary_p1
   {
  public:
     TrackSummary_p1() : m_information(std::vector<int>()), m_idHitPattern(0), m_eProbability(std::vector<float>()), m_pixeldEdx(-1.), m_nhitsfordEdx(-1) {}

     std::vector<int> m_information;
     
     /**contains the 'hit pattern'*/
     unsigned long m_idHitPattern;

     std::vector<float> m_eProbability;

     float m_pixeldEdx;
     int m_nhitsfordEdx;

     TPObjRef       m_indetTrackSummary;
     TPObjRef       m_muonTrackSummary;

   };
}

#endif // TRACK_SUMMARY_P1_TRK_H
