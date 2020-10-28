/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_STANDALONE

#include "GaudiKernel/DeclareFactoryEntries.h"
// #include "ZdcAnalysis/ZdcAnalysisAlg.h"
#include "ZdcAnalysis/ZdcAnalysisTool.h"

// DECLARE_ALGORITHM_FACTORY            (ZDC::ZdcAnalysisAlg)
DECLARE_TOOL_FACTORY            (ZDC::ZdcAnalysisTool)

DECLARE_FACTORY_ENTRIES (ZdcAnalysis) {
  // DECLARE_ALGORITHM      (ZDC::ZdcAnalysisAlg)
  DECLARE_TOOL      (ZDC::ZdcAnalysisTool)
}

#endif
