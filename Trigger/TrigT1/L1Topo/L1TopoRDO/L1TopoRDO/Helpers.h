/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_L1TOPORDOHELPERS_H
#define L1TOPORDO_L1TOPORDOHELPERS_H

#include <vector>
#include <bitset>
#include <utility>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdint>

class L1TopoRDO;
class L1TopoRDOCollection;
class L1TopoResult;

// All helpers
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/L1TopoTOB.h"

/** This file contains some static helper functions to help users of L1TopoRDO */

//! Print helper function for L1TopoRDO
std::ostream& operator<<(std::ostream&, const L1TopoRDO&);

//! Enumeration of block types in the L1Topo data
namespace L1Topo {
  
  //! L1Topo data block types and their 4-bit codes 
  typedef enum BlockTypes {
    HEADER=0xc,
    FIBRE=0xd,
    STATUS=0xe,
    EM_TOB=0x1,
    TAU_TOB=0x2,
    MUON_TOB=0x3,
    JET1_TOB=0x4,
    JET2_TOB=0x5,
    ENERGY_TOB=0x6,
    L1TOPO_TOB=0x8
  } BlockTypes;

  //! Function to return the block type of a data word from L1Topo
  unsigned int blockType(const uint32_t);

  //! Helper function to format a 32-bit integer as an 8-digit hex number for printing 
  std::string formatHex8(uint32_t word);

  //! Helper to calculate the index needed to pack trigger bits into the full 128-bit decision. See 4-bit part of L1Topo TOB definition in https://twiki.cern.ch/twiki/pub/Atlas/L1CaloUpgrade/ROD_data_format_v1.0.4.xlsx

  unsigned int triggerBitIndex(uint32_t moduleId, L1Topo::L1TopoTOB);

  //! Get the trigger decision and overflow bits from the L1Topo ROI data block 'L1Topo TOB' and order them correctly in a bitset. Reference needed for layout.
  //std::pair< std::bitset<128>,std::bitset<128> > getDecisionAndOverflowBits(const std::vector<L1TopoResult>&);
  std::pair< std::bitset<128>,std::bitset<128> > getDecisionAndOverflowBits(const L1TopoRDOCollection&);

} // namespace L1Topo


#endif // L1TOPORDO_L1TOPORDOHELPERS_H
