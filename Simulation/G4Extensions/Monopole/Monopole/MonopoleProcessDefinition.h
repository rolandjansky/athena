/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MonopoleProcessDefinition_H
#define MonopoleProcessDefinition_H

#include "FadsPhysics/UserPhysicsDefinition.h"

class MonopoleProcessDefinition : public UserPhysicsDefinition {

public:
	MonopoleProcessDefinition();
	void ConstructParticle();
	void ConstructProcess();

};

#endif
