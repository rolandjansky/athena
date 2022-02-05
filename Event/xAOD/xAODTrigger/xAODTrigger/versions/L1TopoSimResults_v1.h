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
    unsigned int connectionId() const;
    // Set a human-readable name for the object
    void setConnectionId( const unsigned int id );

    // The "raw" words describing the topological decision or multiplicity bits
    uint32_t topoWord() const;
    // Set the "raw" words describing the topological decision or multiplicity bits
    void setTopoWord( const uint32_t word );

    // The "raw" words describing the topological decision or multiplicity bits
    uint64_t topoWord64() const;
    // Set the "raw" words describing the topological decision or multiplicity bits
    void setTopoWord64( const uint64_t word );

    // The "raw" words describing the topological overflow bits
    uint32_t topoWordOverflow() const;
    // Set the "raw" words describing the topological overflow bits
    void setTopoWordOverflow( const uint32_t word );

    // The "raw" words describing the topological overflow bits
    uint64_t topoWord64Overflow() const;
    // Set the "raw" words describing the topological overflow bits
    void setTopoWord64Overflow( const uint64_t word );

    // The binary length
    unsigned int bitWidth() const; 
    // Set the binary length
    void setBitWidth( const unsigned int bit ); 

    // Get clock information
    unsigned int clock() const;
    // Set clock information
    void setClock( const unsigned int clock );

  }; // class
  
} // namespace xAOD

#endif // XAODTRIGGER_VERSIONS_L1TOPOSIMRESULTS_V1_H
