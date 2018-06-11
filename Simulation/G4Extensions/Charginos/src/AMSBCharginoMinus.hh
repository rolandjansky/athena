/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHARGINOS_AMSBCharginoMinus_H
#define CHARGINOS_AMSBCharginoMinus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class AMSBCharginoMinus: public G4ParticleDefinition
{
private:

  static AMSBCharginoMinus* s_theInstance;
  AMSBCharginoMinus(){}
  ~AMSBCharginoMinus(){}

public:

  static AMSBCharginoMinus*  Definition(G4double mass=-1, G4double width=-1, G4double charge=-1, G4double PDG=-1, G4bool stable=true, G4double lifetime=-1, G4bool shortlived=false);

};
#endif // CHARGINOS_AMSBCharginoMinus_H
