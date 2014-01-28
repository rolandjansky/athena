// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeaderAuxContainer_v1.h 580422 2014-01-28 10:15:11Z krasznaa $
#ifndef XAODTRIGL1CALO_VERSIONS_RODHEADERAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_RODHEADERAUXCONTAINER_V1_H

// STL include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for RODHeader_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 580422 $
  /// $Date: 2014-01-28 11:15:11 +0100 (Tue, 28 Jan 2014) $    

  class RODHeaderAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    RODHeaderAuxContainer_v1();

  private:
    std::vector<uint32_t> version;
    std::vector<uint32_t> sourceId;
    std::vector<uint32_t> run;
    std::vector<uint32_t> lvl1Id;
    std::vector<uint32_t> bcId;
    std::vector<uint32_t> trigType;
    std::vector<uint32_t> detType;
    std::vector<std::vector<uint32_t> > statusWords;
    std::vector<uint32_t> payloadSize;
    
  }; // class RODHeaderAuxContainer_v1 
} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF( xAOD::RODHeaderAuxContainer_v1 , 1110454371 , 1 ) 
      
#include "SGTools/BaseInfo.h"
  SG_BASE( xAOD::RODHeaderAuxContainer_v1 , xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE
#endif // XAODTRIGL1CALO_VERSIONS_RODHEADERAUXCONTAINER_V1_H
