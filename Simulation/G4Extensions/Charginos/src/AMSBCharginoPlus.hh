/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHARGINOS_AMSBCharginoPlus_H
#define CHARGINOS_AMSBCharginoPlus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class AMSBCharginoPlus: public G4ParticleDefinition
{
private:

  static AMSBCharginoPlus* s_theInstance;
  AMSBCharginoPlus(){}
  ~AMSBCharginoPlus(){}

public:

  static AMSBCharginoPlus*  Definition(G4double mass=-1, G4double width=-1, G4double charge=-1, G4double PDG=-1, G4bool stable=true, G4double lifetime=-1, G4bool shortlived=false);

};
#endif // CHARGINOS_AMSBCharginoPlus_H
