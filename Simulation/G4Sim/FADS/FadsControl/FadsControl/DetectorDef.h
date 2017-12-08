/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorConstructionDef_H
#define DetectorConstructionDef_H

#include "FadsControl/DetectorConstructionEntry.h"

#define DETECTORDEFINITION(DET_TYPE,DET_NAME)	\
	static DetectorConstructionEntry<DET_TYPE> detConstr(#DET_NAME);

#endif
