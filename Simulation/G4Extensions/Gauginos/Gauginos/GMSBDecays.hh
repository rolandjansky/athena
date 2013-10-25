/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GMSBDecays_H
#define GMSBDecays_H

#include "FadsPhysics/UserPhysicsDefinition.h"

class GMSBDecays : public UserPhysicsDefinition {

public:
	GMSBDecays();
	void ConstructParticle();
	void ConstructProcess();

};

#endif
