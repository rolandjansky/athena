/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMSBCharginoPlus_H
#define AMSBCharginoPlus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class AMSBCharginoPlus: public G4ParticleDefinition
{
private:

  static AMSBCharginoPlus* theInstance;
  AMSBCharginoPlus(){}
  ~AMSBCharginoPlus(){}

public:

  static AMSBCharginoPlus*  Definition();

};
#endif
