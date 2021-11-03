/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_L1TOPOSIMRESULTS_V1_H
#define XAODTRIGGER_VERSIONS_L1TOPOSIMRESULTS_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>
#include <vector>
#include <bitset>

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

  // Class used to describe L1Topo decision bits
  //
  // This is a generic class for describing the output of L1Topo
  // simulation decisions for all boards
  //
  // @author Anil Sonay <anil.sonay@cern.ch>
  //
  // $Revision: 0 $
  // $D 2021-10-05 $
  class L1TopoSimResults_v1 : public SG::AuxElement {

  public:
    
    // Default constructor
    L1TopoSimResults_v1();
 
    // Get a human-readable name for the object
    std::string boardName() const;
    // Set a human-readable name for the object
    void setName( const std::string name );

    // The "raw" 32/64-bit words describing the topological decision
    uint32_t word32() const;
    uint64_t word64() const;
    std::bitset<128> wordOptical() const;
    
    // Set the "raw" 32/64-bit words describing the topological decision
    void setWord32( const uint32_t value );
    void setWord64( const uint64_t value );
    void setWordOptical( const std::bitset<128> value );

    // Get clock information
    unsigned int clock() const;
    // Set clock information
    void setClock( const unsigned int clock );

  }; // class
  
} // namespace xAOD

#endif // XAODTRIGGER_VERSIONS_L1TOPOSIMRESULTS_V1_H
