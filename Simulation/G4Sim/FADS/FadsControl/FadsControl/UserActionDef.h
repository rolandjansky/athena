/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UserActionDef_H
#define UserActionDef_H

#include "FadsControl/UserActionEntry.h"

#define USERACTIONDEFINITION(DET_TYPE,DET_NAME)	\
	static UserActionEntry<DET_TYPE> uaction(#DET_NAME);

#endif
