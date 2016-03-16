/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROTATEDTRAPEZOID_BOUNDS_P1_TRK_H
#define ROTATEDTRAPEZOID_BOUNDS_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RotatedTrapezoidBounds_p1.h
//
//-----------------------------------------------------------------------------

namespace Trk
{
   class RotatedTrapezoidBounds_p1
   {
  public:
     RotatedTrapezoidBounds_p1() : m_halfX(0.0), m_minHalfY(0.0), m_maxHalfY(0.0), m_kappa(0.0), m_delta(0.0) {}

     float m_halfX;
     float m_minHalfY;
     float m_maxHalfY;
     float m_kappa;
     float m_delta;
   };
}

#endif // TRAPEZOID_BOUNDS_P1_TRK_H
