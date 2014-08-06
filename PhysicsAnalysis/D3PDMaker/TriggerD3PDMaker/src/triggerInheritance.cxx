/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: triggerInheritance.cxx 282405 2010-03-03 09:45:55Z krasznaa $

// Gaudi/Athena include(s):
#include "SGTools/BaseInfo.h"

// EDM include(s):
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "EventKernel/INavigable4Momentum.h"

SG_BASE( Muon_ROI, INavigable4Momentum );
SG_BASE( Jet_ROI, INavigable4Momentum );
SG_BASE( EmTau_ROI, INavigable4Momentum );
