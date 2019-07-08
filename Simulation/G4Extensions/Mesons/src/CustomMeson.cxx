/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CustomMeson.hh"

// ######################################################################
// ###                           CustomMeson                          ###
// ######################################################################

CustomMeson::CustomMeson(const G4String &name, G4double mass, G4double width,
                         G4int charge, G4double spin, G4int parity,
                         G4int isospin3, G4int PDG, G4double lifetime,
                         const G4String &subType)

//    Arguments for constructor are as follows
//               name             mass          width         charge
//             2*spin           parity  C-conjugation
//          2*Isospin       2*Isospin3       G-parity
//               type    lepton number  baryon number   PDG encoding
//             stable         lifetime    decay table
//             shortlived      subType    anti_encoding

  : G4ParticleDefinition( name,        mass,    width,    charge,
                        2*spin,      parity,        0,
                             1,  2*isospin3,        0,
                       "meson",           0,        0,       PDG,
                         false,    lifetime,     NULL,
                         false,     subType) {}
