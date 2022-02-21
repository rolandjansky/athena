/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../PixelClusterAnalysis.h"
#include "../SCT_ClusterAnalysis.h"

DECLARE_ALGORITHM_FACTORY( PixelClusterAnalysis )
DECLARE_ALGORITHM_FACTORY( SCT_ClusterAnalysis )

DECLARE_FACTORY_ENTRIES( TrkClusterAnalysis ) {
    DECLARE_ALGORITHM( PixelClusterAnalysis )
    DECLARE_ALGORITHM( SCT_ClusterAnalysis )
}
