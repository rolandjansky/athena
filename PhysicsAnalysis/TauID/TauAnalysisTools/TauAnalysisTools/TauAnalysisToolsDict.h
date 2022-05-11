/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUANALYSISTOOLSDICT_H
#define TAUANALYSISTOOLS_TAUANALYSISTOOLSDICT_H

#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

#include "TauAnalysisTools/HelperFunctions.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/BuildTruthTaus.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/TauTruthTrackMatchingTool.h"
#include "TauAnalysisTools/CommonDiTauEfficiencyTool.h"
#include "TauAnalysisTools/DiTauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/DiTauTruthMatchingTool.h"
#include "TauAnalysisTools/BuildTruthTaus.h"

#endif // not TAUANALYSISTOOLS_TAUANALYSISTOOLSDICT_H
