/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CustomParticle.hh"

// ######################################################################
// ###                           CustomParticle                       ###
// ######################################################################

CustomParticle::CustomParticle(const G4String &name, G4double mass,
                               G4double width, G4int charge, G4int pdg,
                               G4bool stable, G4double lifetime)

    //    Arguments for constructor are as follows
    //               name             mass          width         charge
    //             2*spin           parity  C-conjugation
    //          2*Isospin       2*Isospin3       G-parity
    //               type    lepton number  baryon number   PDG encoding
    //             stable         lifetime    decay table
    //             shortlived      subType    anti_encoding

    : G4ParticleDefinition(name,     mass, width, charge,
                              0,        0,     0,
                              0,        0,     0,
                        "extra",        0,     0,    pdg,
                         stable, lifetime,  NULL,
                         false) {}
