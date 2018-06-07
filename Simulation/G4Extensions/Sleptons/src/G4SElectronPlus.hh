/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SLEPTONS_G4SElectronPlus_H
#define SLEPTONS_G4SElectronPlus_H

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

  static G4SElectronPlus* Definition(G4double mass=-1, G4double width=-1, G4double charge=-1, G4double PDG=-1, G4bool stable=true, G4double lifetime=-1, G4bool shortlived=false);

};

#endif //SLEPTONS_G4SElectronPlus_H
