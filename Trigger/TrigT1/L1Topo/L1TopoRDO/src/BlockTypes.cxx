/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoRDO/BlockTypes.h"

namespace L1Topo{

  L1Topo::BlockTypes blockType(const uint32_t word){
    return static_cast<BlockTypes>(word>>28 & 0x0f);
  }

} // namespace L1Topo
