/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_P2_TRK_H
#define TRACK_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   Track_p2.h
// Used by both TrackCnv_p12,p2, and p3
//-----------------------------------------------------------------------------

#include <vector>

namespace Trk
{
  class Track_p2
  {
  public:
    Track_p2() {}

    //trackInfo_p1
    TPObjRef                 m_trackInfo;

    // FitQuality_p1
    TPObjRef                 m_fitQuality;

    std::vector< TPObjRef >  m_trackState;
     // TPObjRef m_trackSummary;
  };
}

#endif // TRACK_P1_TRK_H

