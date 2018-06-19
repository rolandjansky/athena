/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROTATED_DIAMOND_BOUNDS_P1_TRK_H
#define ROTATED_DIAMOND_BOUNDS_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RotatedDiamondBounds_p1.h
//
//-----------------------------------------------------------------------------

namespace Trk
{
   class RotatedDiamondBounds_p1
   {
  public:
     RotatedDiamondBounds_p1() : m_minHalfX(0.0), m_medHalfX(0.0), m_maxHalfX(0.0), m_halfY1(0.0), m_halfY2(0.0), m_alpha1(0.0), m_alpha2(0.0) {}

     #ifdef UseFloatsIn_RotatedDiamondBounds
     float m_minHalfX;
     float m_medHalfX;
     float m_maxHalfX;
     float m_halfY1;
     float m_halfY2;
     float m_alpha1;
     float m_alpha2;
     #else
     double m_minHalfX;
     double m_medHalfX;
     double m_maxHalfX;
     double m_halfY1;
     double m_halfY2;
     double m_alpha1;
     double m_alpha2;
     #endif
   };
}

#endif // ROTATED_DIAMOND_BOUNDS_P1_TRK_H
