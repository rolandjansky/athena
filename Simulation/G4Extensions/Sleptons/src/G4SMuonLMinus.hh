/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SLEPTONS_G4SMuonLMinus_H
#define SLEPTONS_G4SMuonLMinus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class G4SMuonLMinus: public G4ParticleDefinition
{
  // singleton implementation

private:

  static G4SMuonLMinus* theInstance;
  G4SMuonLMinus(){}
  ~G4SMuonLMinus(){}

public:

  static G4SMuonLMinus* Definition(G4double mass=-1, G4double width=-1, G4double charge=-1, G4double PDG=-1, G4bool stable=true, G4double lifetime=-1, G4bool shortlived=false);

};

#endif //SLEPTONS_G4SMuonLMinus_H
