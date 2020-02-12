/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CustomMeson_h
#define CustomMeson_h 1

#include "G4ParticleDefinition.hh"
#include "globals.hh"

// ######################################################################
// ###                          CustomMeson                           ###
// ######################################################################

class CustomMeson : public G4ParticleDefinition {

  public:
    CustomMeson(const G4String &name, G4double mass, G4double width,
                G4int charge, G4double spin, G4int parity, G4int isospin3,
                G4int PDG, G4double lifetime, const G4String &subType);

    virtual ~CustomMeson() {}
};

#endif
