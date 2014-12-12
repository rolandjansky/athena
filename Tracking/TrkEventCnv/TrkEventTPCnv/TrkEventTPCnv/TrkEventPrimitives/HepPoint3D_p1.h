/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HepPoint3D_P1_H
#define HepPoint3D_P1_H

class HepPoint3D_p1
{
public:
  HepPoint3D_p1() : m_x(0.0), m_y(0.0), m_z(0.0){}

  #ifdef UseFloatsIn_HepPoint3D
  float m_x, m_y, m_z;
  #else
  double m_x, m_y, m_z;
  #endif
};

#endif

