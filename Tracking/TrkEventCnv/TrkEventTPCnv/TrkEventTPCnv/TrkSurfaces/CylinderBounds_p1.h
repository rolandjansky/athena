/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CYLINDER_BOUNDS_P1_TRK_H
#define CYLINDER_BOUNDS_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   CylinderBounds_p1.h
//
//-----------------------------------------------------------------------------

namespace Trk
{
   class CylinderBounds_p1
   {
  public:
     CylinderBounds_p1() : m_radius(0.0), m_averagePhi(0.0), m_halfPhiSector(0.0), m_halfZ(0.0) {}
            
     #ifdef UseFloatsIn_CylinderBounds
     float m_radius;
     float m_averagePhi;
     float m_halfPhiSector;
     float m_halfZ;
     #else
     double m_radius;
     double m_averagePhi;
     double m_halfPhiSector;
     double m_halfZ;
     #endif 
   };
}

#endif // CYLINDER_BOUNDS_P1_TRK_H
