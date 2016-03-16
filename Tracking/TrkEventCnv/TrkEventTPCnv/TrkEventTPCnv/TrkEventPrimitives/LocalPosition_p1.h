/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   LocalPosition_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef LOCAL_POSITION_P1_TRK_H
#define LOCAL_POSITION_P1_TRK_H

#include <vector>

namespace Trk
{
   class LocalPosition_p1
   {
  public:
     LocalPosition_p1() : m_x(0.0), m_y(0.0){}
     
     #ifdef UseFloatsIn_LocalPosition
     float m_x, m_y;
     #else
     double m_x, m_y;
     #endif
   };
}

#endif // LOCAL_POSITION_P1_TRK_H
