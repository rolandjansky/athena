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

namespace L1Topo {
  class L1TopoTOB;
}

/** This file contains some static helper functions to help users of L1TopoRDO */

//! Print helper function for L1TopoRDO
std::ostream& operator<<(std::ostream&, const L1TopoRDO&);


namespace L1Topo {
  
  //! Helper function to format a 32-bit integer as an 8-digit hex number for printing 
  std::string formatHex8(uint32_t word);
  //! Helper function to format a 32-bit integer as a  4-digit hex number for printing 
  std::string formatHex4(uint32_t word);

  //! OUT OF DATE DO NOT USE Helper to calculate the index needed to pack trigger bits into the full 128-bit decision. See 4-bit part of L1Topo TOB definition in https://twiki.cern.ch/twiki/pub/Atlas/L1CaloUpgrade/ROD_data_format_v1.0.4.xlsx
  unsigned int triggerBitIndex(uint32_t moduleId, L1Topo::L1TopoTOB);

  //! Helper to calculate the index needed to pack trigger bits into the full 128-bit decision. See 4-bit part of L1Topo TOB definition in https://twiki.cern.ch/twiki/pub/Atlas/L1CaloUpgrade/ROD_data_format_v1.0.4.xlsx ; same as TriggerLine::calcCounter()?

  unsigned int triggerBitIndexNew(uint32_t moduleId, L1Topo::L1TopoTOB, unsigned int bitIdx);

  //! Get the trigger decision and overflow bits from the L1Topo ROI data block 'L1Topo TOB' and order them correctly in a bitset. Reference needed for layout.
  //std::pair< std::bitset<128>,std::bitset<128> > getDecisionAndOverflowBits(const std::vector<L1TopoResult>&);
  std::pair< std::bitset<128>,std::bitset<128> > getDecisionAndOverflowBits(const L1TopoRDOCollection&);

} // namespace L1Topo


#endif // L1TOPORDO_L1TOPORDOHELPERS_H
