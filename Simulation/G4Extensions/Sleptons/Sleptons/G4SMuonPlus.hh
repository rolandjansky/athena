/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SMuonPlus_H
#define G4SMuonPlus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class G4SMuonPlus: public G4ParticleDefinition
{
  // singleton implementation

private:

  static G4SMuonPlus* theInstance;
  G4SMuonPlus(){}
  ~G4SMuonPlus(){}
  
public:

  static G4SMuonPlus* Definition();

};

#endif
