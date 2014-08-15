/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ScoringProcessDefinition_H
#define ScoringProcessDefinition_H

#include "FadsPhysics/UserPhysicsDefinition.h"

class ScoringProcessDefinition : public UserPhysicsDefinition {

public:
	ScoringProcessDefinition();
	void ConstructParticle();
	void ConstructProcess();

};

#endif
