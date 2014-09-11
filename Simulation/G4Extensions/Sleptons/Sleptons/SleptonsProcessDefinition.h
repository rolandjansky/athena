/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SleptonsProcessDefinition_H
#define SleptonsProcessDefinition_H

#include "FadsPhysics/UserPhysicsDefinition.h"
#include "G4ParticleDefinition.hh"

class SleptonsProcessDefinition : public UserPhysicsDefinition {

public:
  SleptonsProcessDefinition();
  void ConstructParticle();
  void ConstructProcess();

private:
  G4ParticleDefinition *theSElectronMinus;
  G4ParticleDefinition *theSElectronPlus;
  G4ParticleDefinition *theSMuonMinus;
  G4ParticleDefinition *theSMuonPlus;
  G4ParticleDefinition *theSTauMinus;
  G4ParticleDefinition *theSTauPlus;
};

#endif
