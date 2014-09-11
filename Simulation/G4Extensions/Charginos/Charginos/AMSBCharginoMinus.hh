/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMSBCharginoMinus_H
#define AMSBCharginoMinus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class AMSBCharginoMinus: public G4ParticleDefinition
{
private:

  static AMSBCharginoMinus* theInstance;
  AMSBCharginoMinus(){}
  ~AMSBCharginoMinus(){}

public:

  static AMSBCharginoMinus*  Definition();

};
#endif
