/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryEventRotations_H
#define PrimaryEventRotations_H

#include "FadsKinematics/ParticleManipulator.h"


class PrimaryEventRotations : public FADS::ParticleManipulator {
public:

  PrimaryEventRotations(const std::string& s)
    : ParticleManipulator(s),
      xangle(0), yangle(0), zangle(0)
  {  }

  void AddXRotation(double ax) { xangle = ax; }
  void AddYRotation(double ay) { yangle = ay; }
  void AddZRotation(double az) { zangle = az; }

  // For the C++ layer only
  bool EditParticle(G4PrimaryParticle *);

private:

  double xangle;
  double yangle;
  double zangle;

};

#endif
