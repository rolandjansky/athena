/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SElectronPlus_H
#define G4SElectronPlus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class G4SElectronPlus: public G4ParticleDefinition
{
  // singleton implementation

private:

  static G4SElectronPlus* theInstance;
  G4SElectronPlus(){}
  ~G4SElectronPlus(){}
  
public:

  static G4SElectronPlus* Definition();

};

#endif
