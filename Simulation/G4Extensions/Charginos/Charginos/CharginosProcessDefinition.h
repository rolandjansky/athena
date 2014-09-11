/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CharginosProcessDefinition_H
#define CharginosProcessDefinition_H

#include "FadsPhysics/UserPhysicsDefinition.h"
#include "G4ParticleDefinition.hh"

class CharginosProcessDefinition : public UserPhysicsDefinition {

public:

  CharginosProcessDefinition();
  void ConstructParticle();
  void ConstructProcess();

private:

  G4ParticleDefinition *theCharginoMinus;
  G4ParticleDefinition *theCharginoPlus;
  G4ParticleDefinition *theNeutralino;

};

#endif
