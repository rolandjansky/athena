/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigFTKBankGen/FTKBankGenAlgo.h"
#include "TrigFTKBankGen/FTKConstGenAlgo.h"

#include "TrigFTKBankGen/PattMergeRootAlgo.h"

#include "TrigFTKBankGen/FTKPattGenRootAlgo.h"

#include "TrigFTKBankGen/FTKCachedBankGenAlgo.h"

DECLARE_ALGORITHM_FACTORY(FTKBankGenAlgo)
DECLARE_ALGORITHM_FACTORY(FTKConstGenAlgo)

DECLARE_ALGORITHM_FACTORY(PattMergeRootAlgo)

DECLARE_ALGORITHM_FACTORY(FTKPattGenRootAlgo)

DECLARE_ALGORITHM_FACTORY(FTKCachedBankGenAlgo)

DECLARE_FACTORY_ENTRIES(TrigFTKBankGen) {
  DECLARE_ALGORITHM(FTKBankGenAlgo);
  DECLARE_ALGORITHM(FTKConstGenAlgo);

  DECLARE_ALGORITHM(PattMergeRootAlgo);

  DECLARE_ALGORITHM(FTKTSPBankGenAlgo);

  DECLARE_ALGORITHM(FTKCachedBankGenAlgo);
}
