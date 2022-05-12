/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONE_BOUNDS_P1_TRK_H
#define CONE_BOUNDS_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   ConeBounds_p1.h
//
//-----------------------------------------------------------------------------

namespace Trk
{
   class ConeBounds_p1
   {
  public:
     ConeBounds_p1() : m_alpha(0.0), m_minZ(0.0), m_maxZ(0.0), 
                       m_avePhi(0.0), m_halfPhiSector(0.0) {}

     float m_alpha;
     float m_minZ;
     float m_maxZ;
     float m_avePhi;
     float m_halfPhiSector;

   };
}

#endif // CONE_BOUNDS_P1_TRK_H
