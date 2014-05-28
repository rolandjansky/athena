/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef globals_H
#define globals_H

#include "AGDD2Geo/AddMaterial.h"
#include "AGDD2Geo/AGDDPositioner.h"

struct globals {
	static AddMaterial addMaterial;
	static AGDDPositioner *currentPositioner;
};

#endif
