/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsListDef_H
#define PhysicsListDef_H

#include "FadsControl/PhysicsListEntry.h"

#define PHYSICSLISTDEFINITION(DET_TYPE,DET_NAME)	\
	static PhysicsListEntry<DET_TYPE> physList(#DET_NAME);

#endif
