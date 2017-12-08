/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryGeneratorDef_H
#define PrimaryGeneratorDef_H

#include "FadsControl/PrimaryGeneratorEntry.h"

#define PRIMARYGENERATORDEFINITION(DET_TYPE,DET_NAME)	\
	static PrimaryGeneratorEntry<DET_TYPE> primGenDef(#DET_NAME);

#endif
