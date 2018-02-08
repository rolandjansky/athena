/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4BARREL_LArStraightElectrodes_H
#define LARG4BARREL_LArStraightElectrodes_H

#include "PhysicalVolumeAccessor.h"
#include <string>

class LArStraightElectrodes {
private:
  static PhysicalVolumeAccessor* s_theElectrodes;
  double m_xcent[1024][14];
  double m_ycent[1024][14];
  double m_cosu[1024][14];
  double m_sinu[1024][14];
  double m_halflength[1024][14];
  bool m_filled;
  static LArStraightElectrodes* s_instance;
  int m_parity;
public:
  static LArStraightElectrodes* GetInstance(std::string strDetector="") ;
  double XCentEle(int stackid, int cellid) const;
  double YCentEle(int stackid, int cellid) const;
  double SlantEle(int stackid, int cellid) const;
  double HalfLength(int stackid, int cellid) const;
  double Cosu(int stackid, int cellid) const;
  double Sinu(int stackid, int cellid) const;
protected:
  LArStraightElectrodes(std::string strDetector="");
};

#endif // LARG4BARREL_LArStraightElectrodes_H
