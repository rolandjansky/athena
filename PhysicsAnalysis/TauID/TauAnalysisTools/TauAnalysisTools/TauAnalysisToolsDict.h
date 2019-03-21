/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUANALYSISTOOLSDICT_H
#define TAUANALYSISTOOLS_TAUANALYSISTOOLSDICT_H




/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

#include "TauAnalysisTools/HelperFunctions.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/TauTruthTrackMatchingTool.h"
#include "TauAnalysisTools/CommonDiTauEfficiencyTool.h"
#include "TauAnalysisTools/DiTauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/CommonDiTauSmearingTool.h"
#include "TauAnalysisTools/DiTauSmearingTool.h"
#include "TauAnalysisTools/DiTauTruthMatchingTool.h"
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"

#endif // not TAUANALYSISTOOLS_TAUANALYSISTOOLSDICT_H
