/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GMSBNEUTRALINO_H
#define GMSBNEUTRALINO_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class GMSBNeutralino: public G4ParticleDefinition
{
private:

  static GMSBNeutralino* theInstance;
  GMSBNeutralino(){}
  ~GMSBNeutralino(){}

public:

  static GMSBNeutralino* Definition();

};
#endif

