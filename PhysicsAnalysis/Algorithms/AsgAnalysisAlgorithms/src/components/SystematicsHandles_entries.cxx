/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#include <GaudiKernel/DeclareFactoryEntries.h>

#include <AsgAnalysisAlgorithms/SysListLoaderAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::SysListLoaderAlg)

DECLARE_FACTORY_ENTRIES(AsgAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::SysListLoaderAlg)
}

