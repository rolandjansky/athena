/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
private:
  static bool loaded;
  static std::set<G4ParticleDefinition *> m_particles;

public:
  static void loadCustomParticles();
  static bool isCustomParticle(G4ParticleDefinition *particle);


};


#endif
