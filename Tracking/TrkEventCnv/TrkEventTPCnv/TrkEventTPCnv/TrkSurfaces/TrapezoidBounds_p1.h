/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRAPEZOID_BOUNDS_P1_TRK_H
#define TRAPEZOID_BOUNDS_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrapezoidBounds_p1.h
//
//-----------------------------------------------------------------------------

namespace Trk
{
   class TrapezoidBounds_p1
   {
  public:
     TrapezoidBounds_p1() : m_minHalfX(0.0), m_maxHalfX(0.0), m_halfY(0.0), m_alpha(0.0), m_beta(0.0) {}

     #ifdef UseFloatsIn_TrapezoidBounds
     float m_minHalfX;
     float m_maxHalfX;
     float m_halfY;
     float m_alpha;
     float m_beta;
     #else
     double m_minHalfX;
     double m_maxHalfX;
     double m_halfY;
     double m_alpha;
     double m_beta;
     #endif
   };
}

#endif // TRAPEZOID_BOUNDS_P1_TRK_H
