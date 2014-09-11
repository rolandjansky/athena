/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4STauPlus_H
#define G4STauPlus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class G4STauPlus: public G4ParticleDefinition
{
  // singleton implementation

private:

  static G4STauPlus* theInstance;
  G4STauPlus(){}
  ~G4STauPlus(){}
  
public:

  static G4STauPlus* Definition();

};

#endif
