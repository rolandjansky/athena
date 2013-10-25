/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iomanip>

#include "G4AtlasControl/ParticleDataModifier.h"

#include "Gauginos/GMSBGravitino.hh"
// ######################################################################
// ###                         Gravitino                             ###
// ######################################################################

GMSBGravitino* GMSBGravitino::theInstance = Definition();

GMSBGravitino* GMSBGravitino::Definition()
{
  if (theInstance != 0) return theInstance;

  double GravitinoMass = 0.108E-04*CLHEP::GeV;

  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table
  //             shortlived      subType    anti_encoding

  G4ParticleDefinition* anInstance =
    new G4ParticleDefinition("s_G",              GravitinoMass,  0.0*CLHEP::MeV,   0,
                             3,                  0,              0,
                             0,                  0,              0,
                             "supersymmetric",   0,              0,         1000039,
                             true,               -1.0,           NULL,
                             false,              "Gravitino");

  theInstance = reinterpret_cast<GMSBGravitino*>(anInstance);
  return theInstance;
}
