/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RunManagerDef_H
#define RunManagerDef_H

#include "FadsControl/RunManagerEntry.h"

#define RUNMANAGERDEFINITION(RM_TYPE,RM_NAME)	\
	static RunManagerEntry<RM_TYPE> runManager(#RM_NAME);

#endif
