/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RHadronProcessDefinition_H
#define RHadronProcessDefinition_H

#include "FadsPhysics/UserPhysicsDefinition.h"

class RHadronProcessDefinition : public UserPhysicsDefinition {

public:
  RHadronProcessDefinition();
  void ConstructParticle();
  void ConstructProcess();

};

#endif
