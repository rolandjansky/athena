/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VertexRangeChecker_H
#define VertexRangeChecker_H

#include "FadsKinematics/VertexManipulator.h"

class G4VSolid;

class VertexRangeChecker : public FADS::VertexManipulator {
public:

  VertexRangeChecker(const std::string& s)
    : VertexManipulator(s),
      m_Z(-1),
      m_Rmax(-1),
      m_Rmin(0),
      m_sWorld(0)
  {}

  void SetZ(double z) { m_Z = z; }
  double GetZ() const { return m_Z; }

  void SetRmin(double r) { m_Rmin = r; }
  double GetRmin() const { return m_Rmin; }

  void SetRmax(double R) { m_Rmax = R; }
  double GetRmax() const { return m_Rmax; }

  // For the C++ layer only
  bool EditVertex(G4PrimaryVertex *);

private:

  double m_Z, m_Rmax, m_Rmin;
  G4VSolid * m_sWorld;

};

#endif
