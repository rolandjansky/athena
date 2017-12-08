/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SteppingVerboseDef_H
#define SteppingVerboseDef_H

#include "G4VSteppingVerbose.hh"

#define STEPPINGVERBOSEDEFINITION(XX_TYPE)	\
struct steppingVerbose {												\
	steppingVerbose() {G4VSteppingVerbose::SetInstance(new XX_TYPE);}	\
};																		\
static steppingVerbose a;

#endif
