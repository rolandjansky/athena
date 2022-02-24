/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../SpacePointAnalysis.h"

DECLARE_ALGORITHM_FACTORY( SpacePointAnalysis )

DECLARE_FACTORY_ENTRIES( TrkSpacePointAnalysis ) {
    DECLARE_ALGORITHM( SpacePointAnalysis )
}
