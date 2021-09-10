/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "HGTD_PRDFormation/PadClusterizationAlg.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(HGTD, PadClusterizationAlg)

DECLARE_FACTORY_ENTRIES(HGTD_PRDFormation) {
  DECLARE_NAMESPACE_ALGORITHM(HGTD, PadClusterizationAlg)
}
