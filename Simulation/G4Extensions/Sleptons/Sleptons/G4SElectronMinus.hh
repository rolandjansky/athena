/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SElectronMinus_H
#define G4SElectronMinus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class G4SElectronMinus: public G4ParticleDefinition
{
  // singleton implementation

private:

  static G4SElectronMinus* theInstance;
  G4SElectronMinus(){}
  ~G4SElectronMinus(){}
  
public:

  static G4SElectronMinus* Definition();

};

#endif
