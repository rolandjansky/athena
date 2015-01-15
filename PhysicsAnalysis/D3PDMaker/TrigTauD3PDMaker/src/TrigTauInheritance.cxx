/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "TrigParticle/TrigTau.h"
#include "EventKernel/INavigable4Momentum.h"
#include "SGTools/BaseInfo.h"

/* add class types to storegate identifing inheritance of classes */
SG_ADD_BASE (EmTau_ROI, SG_VIRTUAL (INavigable4Momentum));
SG_ADD_BASE (TrigTau, SG_VIRTUAL (INavigable4Momentum));
