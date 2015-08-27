/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_P1_TRK_H
#define TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   Track_p1.h
//
//-----------------------------------------------------------------------------

#include <vector>

namespace Trk
{
   class Track_p1
   {
  public:
     Track_p1() : m_author(0), m_particleHypo(0) {}

     unsigned int	m_author;
     unsigned int	m_particleHypo;
     // FitQuality_p1
     TPObjRef		m_fitQuality;
     // std :: vector<TrackStateOnSurface_p1>
     std::vector< TPObjRef >	m_trackState;
   };
}

#endif // TRACK_P1_TRK_H

