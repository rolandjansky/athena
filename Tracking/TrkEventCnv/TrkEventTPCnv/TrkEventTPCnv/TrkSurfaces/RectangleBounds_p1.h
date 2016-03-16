/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTANGLE_BOUNDS_P1_TRK_H
#define RECTANGLE_BOUNDS_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RectangleBounds_p1.h
//
//-----------------------------------------------------------------------------

namespace Trk
{
   class RectangleBounds_p1
   {
  public:
     RectangleBounds_p1() : m_halfX(0.0), m_halfY(0.0) {}

     #ifdef UseFloatsIn_RectangleBounds
     float m_halfX;
     float m_halfY;
     #else
     double m_halfX;
     double m_halfY;
     #endif
   };
}

#endif // RECTANGLE_BOUNDS_P1_TRK_H
