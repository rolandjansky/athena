/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigMinBias/versions/TrigSpacePointCountsAuxContainer_v1.h"

namespace xAOD {

  TrigSpacePointCountsAuxContainer_v1::TrigSpacePointCountsAuxContainer_v1 ()
    : AuxContainerBase() {

    AUX_VARIABLE ( contentsPixelClusEndcapC );
    AUX_VARIABLE ( contentsPixelClusBarrel );
    AUX_VARIABLE ( contentsPixelClusEndcapA );
    AUX_VARIABLE ( pixelClusTotBins );
    AUX_VARIABLE ( pixelClusTotMin );
    AUX_VARIABLE ( pixelClusTotMax );
    AUX_VARIABLE ( pixelClusSizeBins );
    AUX_VARIABLE ( pixelClusSizeMin );
    AUX_VARIABLE ( pixelClusSizeMax );
    AUX_VARIABLE ( sctSpEndcapC );
    AUX_VARIABLE ( sctSpBarrel );
    AUX_VARIABLE ( sctSpEndcapA );
  }

}
