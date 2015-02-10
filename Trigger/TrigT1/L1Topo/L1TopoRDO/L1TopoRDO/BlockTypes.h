/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_BLOCKTYPES_H
#define L1TOPORDO_BLOCKTYPES_H

#include <cstdint>


namespace L1Topo {

  //! L1Topo data block types and their 4-bit codes 
  enum class BlockTypes {
    HEADER=0xc,
    FIBRE=0xd,
    STATUS=0xe,
    EM_TOB=0x0,
    TAU_TOB=0x1,
    MUON_TOB=0x2,
    JET1_TOB=0x4,
    JET2_TOB=0x5,
    ENERGY_TOB=0x6,
    L1TOPO_TOB=0x8
  };

  //! Function to return the block type of a data word from L1Topo   
  L1Topo::BlockTypes blockType(const uint32_t);

} // namespace L1Topo

#endif // L1TOPORDO_BLOCKTYPES_H
