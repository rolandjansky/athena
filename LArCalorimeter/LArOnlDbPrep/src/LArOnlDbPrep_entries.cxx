/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArOnlDbPrep/LArOnlDbPrepTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "LArOnlDbPrep/LArDSPThresholdFillAlg.h"
#include "LArOnlDbPrep/LArOnlDbPrepAlg.h"

DECLARE_TOOL_FACTORY( LArOnlDbPrepTool )
DECLARE_ALGORITHM_FACTORY ( LArDSPThresholdFillAlg )
DECLARE_ALGORITHM_FACTORY ( LArOnlDbPrepAlg )

DECLARE_FACTORY_ENTRIES(LArOnlDbPrep) {
DECLARE_TOOL( LArOnlDbPrepTool )
DECLARE_ALGORITHM( LArDSPThresholdFillAlg )
DECLARE_ALGORITHM ( LArOnlDbPrepAlg )
}
