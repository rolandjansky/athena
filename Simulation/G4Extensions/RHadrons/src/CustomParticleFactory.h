/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CustomParticleFactory_h
#define CustomParticleFactory_h 1

#include "CustomParticle.h"
#include <set>


// ######################################################################
// ###                          CustomParticle                                ###
// ######################################################################

class CustomParticleFactory
{

public:
  static void loadCustomParticles();
  static bool isCustomParticle(G4ParticleDefinition *particle);

private:
  static std::set<G4ParticleDefinition *> load();
};


#endif
