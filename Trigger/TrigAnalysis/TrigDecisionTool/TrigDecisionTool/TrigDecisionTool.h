/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDecision_TrigDecisionTool_h
#define TrigDecision_TrigDecisionTool_h

#include "AsgTools/AsgToolsConf.h"

//full Athena
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
#include "TrigDecisionTool/TrigDecisionToolFullAthena.h"
#endif

// AthAnalysisBase
#if defined(ASGTOOL_ATHENA) && defined(XAOD_ANALYSIS)
#include "TrigDecisionTool/TrigDecisionToolStandalone.h"
#endif

// ASG release
#ifdef ASGTOOL_STANDALONE
#include "TrigDecisionTool/TrigDecisionToolStandalone.h"
#endif

#endif
