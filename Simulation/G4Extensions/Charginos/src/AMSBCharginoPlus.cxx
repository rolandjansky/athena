/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Charginos/AMSBCharginoPlus.hh"
#include "CLHEP/Units/PhysicalConstants.h"

// ######################################################################
// ###                      CharginoPlus                              ###
// ######################################################################

AMSBCharginoPlus* AMSBCharginoPlus::theInstance = Definition();

AMSBCharginoPlus* AMSBCharginoPlus::Definition()
{
  if (theInstance != 0) return theInstance;

  double CharginoMass = 101.0*CLHEP::GeV;

  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table
  //             shortlived      subType    anti_encoding

  G4ParticleDefinition* anInstance =
    new G4ParticleDefinition("s_chi_plus_1",     CharginoMass,    0.0*CLHEP::MeV,    +1.*CLHEP::eplus,
                             1,                  0,               0,
                             0,                  0,               0,
                             "supersymmetric",   0,               0,          1000024,
                             true,               -1.0,            NULL,
                             false,              "CharginoPlus");

 theInstance = reinterpret_cast<AMSBCharginoPlus*>(anInstance);
 return theInstance;
}
