/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_P12_TRK_H
#define TRACK_P12_TRK_H

#include <vector>

#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"

namespace Trk
{
   class Track_p12
   {
  public:
     Track_p12() {}

     unsigned int	m_author;
     unsigned int	m_particleHypo;
     FitQuality_p1  m_fitQuality;
     // std :: vector<TrackStateOnSurface_p1>
     std::vector< TPObjRef >	m_trackState;
   };
}
#endif 

