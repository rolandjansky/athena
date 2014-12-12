/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_STATE_ON_SURFACE_P3_TRK_H
#define TRACK_STATE_ON_SURFACE_P3_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackStateOnSurface_p3.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
    class TrackStateOnSurface_p3
    {
        public:
      TrackStateOnSurface_p3() : m_typeFlags(0) {}

      // TrackParameters_p2 
      TPObjRef  m_trackParameters;
      // FitQualityOnSurface_p1 
      TPObjRef  m_fitQualityOnSurface;
      // MaterialEffectsBase_p1 
      TPObjRef  m_materialEffects;
      // MeasurementBase_p1 
      TPObjRef  m_measurementOnTrack;
      long      m_typeFlags;
    };
}

#endif // TRACK_STATE_ON_SURFACE_P1_TRK_H
