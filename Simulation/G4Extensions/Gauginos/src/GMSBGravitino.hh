/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GAUGINOS_GMSBGRAVITINO_H
#define GAUGINOS_GMSBGRAVITINO_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class GMSBGravitino: public G4ParticleDefinition
{
private:

  static GMSBGravitino* s_theInstance;
  GMSBGravitino(){}
  ~GMSBGravitino(){}

public:

  static GMSBGravitino* Definition(G4double mass=-1, G4double width=-1, G4double charge=-1, G4double PDG=-1, G4bool stable=true, G4double lifetime=-1, G4bool shortlived=false);

};
#endif //GAUGINOS_GMSBGRAVITINO_H
