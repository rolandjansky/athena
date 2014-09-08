/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_SUMMARY_P2_TRK_H
#define TRACK_SUMMARY_P2_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class TrackSummary_p2
   {
  public:
     TrackSummary_p2() : m_information(std::vector<int>()), m_idHitPattern(0), m_eProbability(std::vector<float>()), m_pixeldEdx(-1.), m_nhitsfordEdx(-1) {}

     std::vector<int> m_information;
     
     /**contains the 'hit pattern'*/
     unsigned long m_idHitPattern;

     std::vector<float> m_eProbability;

     float m_pixeldEdx;
     int m_nhitsfordEdx;

     std::vector<float> m_indetTrackSummary;
     std::vector<unsigned int> m_muonTrackSummary;

   };
}

#endif