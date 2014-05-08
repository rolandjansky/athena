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
      Z(-1),
      Rmax(-1),
      Rmin(0),
      m_sWorld(0)
  {}

  void SetZ(double z) { Z = z; }
  double GetZ() const { return Z; }

  void SetRmin(double r) { Rmin = r; }
  double GetRmin() const { return Rmin; }

  void SetRmax(double R) { Rmax = R; }
  double GetRmax() const { return Rmax; }

  // For the C++ layer only
  bool EditVertex(G4PrimaryVertex *);

private:

  double Z, Rmax, Rmin;
  G4VSolid * m_sWorld;

};

#endif
