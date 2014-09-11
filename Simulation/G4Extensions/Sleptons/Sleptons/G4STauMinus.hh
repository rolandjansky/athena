/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4STauMinus_H
#define G4STauMinus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class G4STauMinus: public G4ParticleDefinition
{
  // singleton implementation

private:

  static G4STauMinus* theInstance;
  G4STauMinus(){}
  ~G4STauMinus(){}
  
public:

  static G4STauMinus* Definition();

};

#endif
