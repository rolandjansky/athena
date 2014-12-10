/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef globals_H
#define globals_H

#include "AGDDHandlers/AddMaterial.h"
#include "AGDDKernel/AGDDPositioner.h"

struct globals {
	static AddMaterial addMaterial;
	static AGDDPositioner *currentPositioner;
};

#endif
