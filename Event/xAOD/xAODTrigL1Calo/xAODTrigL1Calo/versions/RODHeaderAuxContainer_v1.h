// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeaderAuxContainer_v1.h 631511 2014-11-27 13:17:07Z gwatts $
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
  /// $Revision: 631511 $
  /// $Date: 2014-11-27 14:17:07 +0100 (Thu, 27 Nov 2014) $    

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

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::RODHeaderAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_RODHEADERAUXCONTAINER_V1_H
