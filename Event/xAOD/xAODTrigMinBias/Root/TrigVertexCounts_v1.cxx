/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCore/AuxStoreAccessorMacros.h"

#include "xAODTrigMinBias/versions/TrigVertexCounts_v1.h"


namespace xAOD {

  // Default ctor
  TrigVertexCounts_v1::TrigVertexCounts_v1()
    : SG::AuxElement() {
  }

  TrigVertexCounts_v1::~TrigVertexCounts_v1(){}

  AUXSTORE_OBJECT_SETTER_AND_GETTER (TrigVertexCounts_v1, std::vector<unsigned int> , vtxNtrks, setVtxNtrks)
  AUXSTORE_OBJECT_SETTER_AND_GETTER (TrigVertexCounts_v1, std::vector<float>, vtxTrkPtSqSum, setVtxTrkPtSqSum )
  
}
