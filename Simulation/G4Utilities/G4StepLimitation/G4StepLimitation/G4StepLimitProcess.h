/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4StepLimitProcess_H
#define G4StepLimitProcess_H

#include "FadsPhysics/UserPhysicsDefinition.h"

class G4StepLimitProcess : public UserPhysicsDefinition {

public:
	G4StepLimitProcess();
	void ConstructParticle();
	void ConstructProcess();

};

#endif
