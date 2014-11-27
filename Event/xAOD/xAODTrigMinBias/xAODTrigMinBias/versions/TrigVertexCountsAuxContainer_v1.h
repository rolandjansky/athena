/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigVertexCountsAUXCONTAINTER_V1_H
#define xAODTrigMinBias_TrigVertexCountsAUXCONTAINTER_V1_H

#include "xAODCore/AuxContainerBase.h"

#include <vector>

namespace xAOD {

  class TrigVertexCountsAuxContainer_v1 : public AuxContainerBase {

  public:
    TrigVertexCountsAuxContainer_v1();

  private:
    
    std::vector< std::vector<unsigned int> > vtxNtrks;
    
    std::vector< std::vector<float> > vtxTrkPtSqSum;

  };

}

#include "xAODCore/BaseInfo.h"
SG_BASE(   xAOD::TrigVertexCountsAuxContainer_v1, xAOD::AuxContainerBase );

#endif
