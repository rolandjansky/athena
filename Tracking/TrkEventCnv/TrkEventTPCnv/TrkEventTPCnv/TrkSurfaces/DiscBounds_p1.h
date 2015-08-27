/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DISC_BOUNDS_P1_TRK_H
#define DISC_BOUNDS_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   DiscBounds_p1.h
//
//-----------------------------------------------------------------------------

namespace Trk
{
   class DiscBounds_p1
   {
  public:
     DiscBounds_p1() : m_rMin(0.0), m_rMax(0.0), m_avePhi(0.0), m_hPhiSec(0.0) {}

     #ifdef UseFloatsIn_DiscBounds
     float m_rMin;
     float m_rMax;
     float m_avePhi;
     float m_hPhiSec;
     #else
     double m_rMin;
     double m_rMax;
     double m_avePhi;
     double m_hPhiSec;
     #endif
   };
}

#endif // DISC_BOUNDS_P1_TRK_H
