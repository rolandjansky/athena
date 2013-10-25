/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GMSBGRAVITINO_H
#define GMSBGRAVITINO_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class GMSBGravitino: public G4ParticleDefinition
{
private:

  static GMSBGravitino* theInstance;
  GMSBGravitino(){}
  ~GMSBGravitino(){}
  
public:

  static GMSBGravitino* Definition();

};
#endif
