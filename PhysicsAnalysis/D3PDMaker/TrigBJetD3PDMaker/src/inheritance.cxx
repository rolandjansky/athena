/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/BaseInfo.h"
#include "EventKernel/INavigable4Momentum.h"

#include "TrigParticle/TrigL2Bjet.h"
#include "TrigParticle/TrigEFBjet.h"

SG_ADD_BASE(TrigL2Bjet, SG_VIRTUAL (INavigable4Momentum));
SG_ADD_BASE(TrigEFBjet, SG_VIRTUAL (INavigable4Momentum));

