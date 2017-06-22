/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4BARREL_LArStraightAbsorbers_H
#define LARG4BARREL_LArStraightAbsorbers_H

#include "PhysicalVolumeAccessor.h"
#include <string>

class LArStraightAbsorbers {
private:
  static PhysicalVolumeAccessor* s_theAbsorbers;
  double m_xcent[1024][14];
  double m_ycent[1024][14];
  double m_cosu[1024][14];
  double m_sinu[1024][14];
  double m_halflength[1024][14];
  bool m_filled;
  static LArStraightAbsorbers* s_instance;
  int m_parity;
public:
  static LArStraightAbsorbers* GetInstance(std::string strDetector="") ;
  double XCentAbs(int stackid, int cellid) const;
  double YCentAbs(int stackid, int cellid) const;
  double SlantAbs(int stackid, int cellid) const;
  double HalfLength(int stackid, int cellid) const;
  double Cosu(int stackid, int cellid) const;
  double Sinu(int stackid, int cellid) const;
protected:
  LArStraightAbsorbers(std::string strDetector="") ;
};

#endif // LARG4BARREL_LArStraightAbsorbers_H
