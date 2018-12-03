/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "src/GMSBNeutralino.hh"
// ######################################################################
// ###                         Neutralino                             ###
// ######################################################################

GMSBNeutralino* GMSBNeutralino::s_theInstance = NULL;

GMSBNeutralino* GMSBNeutralino::Definition(G4double mass, G4double width, G4double charge, G4double PDG, G4bool stable, G4double lifetime, G4bool shortlived)
{

  if (s_theInstance !=0 && (mass>=0. || width>=0. || lifetime>=0.) )
    {
      G4ExceptionDescription description;
      description << "Trying to redefine the GMSB Neutralino properties after it has been constructed is not allowed";
      G4Exception("GMSBNeutralino", "FailedRedefinition", FatalException, description);
      abort();
    }

  if (s_theInstance != 0)
    {
      return s_theInstance;
    }

  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table
  //             shortlived      subType    anti_encoding
  if (mass >= 0.)
    {

      G4ParticleDefinition* anInstance = new G4ParticleDefinition("s_chi_0_1",       mass,  width,   charge,
                                                                  1,                 0,                0,
                                                                  0,                 0,                0,
                                                                  "supersymmetric",  0,                0,         PDG,
                                                                  stable,              lifetime,             NULL,
                                                                  shortlived,             "Neutralino");

      s_theInstance = reinterpret_cast<GMSBNeutralino*>(anInstance);
      return s_theInstance;
    }
  else
    {
      G4ExceptionDescription description;
      description << "Trying to create a particle with default constructor is not allowed";
      G4Exception("GMSBNeutralino", "DefaultConstructorCalled", FatalException, description);
      abort();
    }
}
