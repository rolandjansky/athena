/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

    
#include "Sleptons/G4SElectronMinus.hh"
// ######################################################################
// ###                         SElectronMinus                              ###
// ######################################################################

G4SElectronMinus* G4SElectronMinus::theInstance = Definition();

G4SElectronMinus* G4SElectronMinus::Definition()
{
  if (theInstance !=0) return theInstance;

//    Arguments for constructor are as follows
//               name             mass          width         charge
//             2*spin           parity  C-conjugation
//          2*Isospin       2*Isospin3       G-parity
//               type    lepton number  baryon number   PDG encoding
//             stable         lifetime    decay table
//             shortlived      subType    anti_encoding

  G4ParticleDefinition* anInstance = new G4ParticleDefinition(
        "s_e_minus_R",      100.00*CLHEP::GeV,       0.0*CLHEP::MeV,    -1.*CLHEP::eplus, 
		    0,               0,             0,          
		    0,               0,             0,             
	    "slepton",               1,             0,      2000011,
     	         true,            -1.0,          NULL,
         	false,"SElectronMinus");

  theInstance = reinterpret_cast<G4SElectronMinus*>(anInstance);
  return theInstance;
}
