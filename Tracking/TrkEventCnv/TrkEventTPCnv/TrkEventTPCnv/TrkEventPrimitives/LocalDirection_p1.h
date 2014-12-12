/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   LocalDirection_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef LOCAL_DIRECTION_P1_TRK_H
#define LOCAL_DIRECTION_P1_TRK_H

namespace Trk
{
    class LocalDirection_p1
    {
   public:
      LocalDirection_p1() : m_angleXZ(0.0), m_angleYZ(0.0) {}

      #ifdef UseFloatsIn_LocalDirection
      float m_angleXZ;
      float m_angleYZ;
      #else
      double m_angleXZ;
      double m_angleYZ;
      #endif
    };
}

#endif // LOCAL_DIRECTION_P1_TRK_H
