/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SMuonMinus_H
#define G4SMuonMinus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class G4SMuonMinus: public G4ParticleDefinition
{
  // singleton implementation

private:

  static G4SMuonMinus* theInstance;
  G4SMuonMinus(){}
  ~G4SMuonMinus(){}
  
public:

  static G4SMuonMinus* Definition();

};

#endif
