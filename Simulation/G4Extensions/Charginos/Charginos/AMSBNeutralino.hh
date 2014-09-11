/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMSBNeutralino_H
#define AMSBNeutralino_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class AMSBNeutralino: public G4ParticleDefinition
{
private:

  static AMSBNeutralino* theInstance;
  AMSBNeutralino(){}
  ~AMSBNeutralino(){}
  
public:

  static AMSBNeutralino* Definition();

};
#endif
