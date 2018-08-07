/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_STANDALONE

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ZdcAnalysis/ZdcAnalysisTool.h"

DECLARE_TOOL_FACTORY            (ZDC::ZdcAnalysisTool)

DECLARE_FACTORY_ENTRIES (ZdcAnalysis) {
  DECLARE_TOOL      (ZDC::ZdcAnalysisTool)
}

#endif
