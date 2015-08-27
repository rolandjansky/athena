/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCATTERING_ANGLE_ON_TRACK_P1_TRK_H
#define SCATTERING_ANGLE_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   ScatteringAngleOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

namespace Trk
{
   class ScatteringAngleOnTrack_p1
   {
  public:
     ScatteringAngleOnTrack_p1() : m_deltaPhi(0.0), m_deltaTheta(0.0), m_sigmaDeltaPhi(0.0), m_sigmaDeltaTheta(0.0) {}

     double m_deltaPhi;
     double m_deltaTheta;
     double m_sigmaDeltaPhi;
     double m_sigmaDeltaTheta;
   };
}

#endif // SCATTERING_ANGLE_ON_SURFACE_P1_TRK_H
