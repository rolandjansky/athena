/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXTRA_PARTICLES__CUSTOM_PARTICLE_H
#define EXTRA_PARTICLES__CUSTOM_PARTICLE_H 1

#include "G4ParticleDefinition.hh"
#include "globals.hh"

// ######################################################################
// ###                           CustomParticle                       ###
// ######################################################################

namespace ExtraParticles {

class CustomParticle : public G4ParticleDefinition {

  public:
    CustomParticle(const G4String &name, G4double mass, G4double width,
                   G4int charge, G4int pdg, G4bool stable, G4double lifetime);

    virtual ~CustomParticle() {}
};

} // namespace ExtraParticles

#endif // EXTRA_PARTICLES__CUSTOM_PARTICLE_H
