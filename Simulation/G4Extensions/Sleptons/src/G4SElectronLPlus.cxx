/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4SElectronLPlus.hh"
// ######################################################################
// ###                         SElectronLPlus                              ###
// ######################################################################


G4SElectronLPlus* G4SElectronLPlus::theInstance = NULL;

G4SElectronLPlus* G4SElectronLPlus::Definition(G4double mass, G4double width, G4double charge, G4double PDG, G4bool stable, G4double lifetime, G4bool shortlived)
{

  if (theInstance !=0 && (mass>=0. || width>=0. || lifetime>=0.) )
    {
      G4ExceptionDescription description;
      description << "Trying to redefine the G4SElectronL Plus properties after it has been constructed is not allowed";
      G4Exception("G4SElectronLPlus", "FailedRedefinition", FatalException, description);
      abort();
    }

  if (theInstance != 0)
    {
      return theInstance;
    }


  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table
  //             shortlived      subType    anti_encoding
  if (mass >= 0) {

    G4ParticleDefinition* anInstance =
      new G4ParticleDefinition("s_e_plus_L",     mass,    width,    charge,
                               0,                  0,               0,
                               0,                  0,               0,
                               "slepton",   1,               0,          PDG,
                               stable,               lifetime,            NULL,
                               shortlived,              "SElectronLPlus");

    theInstance = reinterpret_cast<G4SElectronLPlus*>(anInstance);
    return theInstance;
  }
  else
    {
      G4ExceptionDescription description;
      description << "Trying to create a particle with default constructor is not allowed";
      G4Exception("G4SElectronLPlus", "DefaultConstructorCalled", FatalException, description);
      abort();
    }
}
