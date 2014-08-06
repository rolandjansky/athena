/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigMinBias/versions/TrigVertexCountsAuxContainer_v1.h"

namespace xAOD {

  TrigVertexCountsAuxContainer_v1::TrigVertexCountsAuxContainer_v1 ()
    : AuxContainerBase() {

      AUX_VARIABLE ( vtxNtrks );
      AUX_VARIABLE ( vtxTrkPtSqSum );

  }

}
