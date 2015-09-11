// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeaderAuxContainer_v2.h 694350 2015-09-11 12:02:19Z krasznaa $
#ifndef XAODTRIGL1CALO_VERSIONS_RODHEADERAUXCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_RODHEADERAUXCONTAINER_V2_H

// STL include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for RODHeader_v2
  ///
  /// @author alexander.mazurov@cern.ch
  ///
  /// $Revision: 694350 $
  /// $Date: 2015-09-11 14:02:19 +0200 (Fri, 11 Sep 2015) $    

  class RODHeaderAuxContainer_v2 : public AuxContainerBase {
  public:
    // Default constructor
    RODHeaderAuxContainer_v2();

  private:
    std::vector<uint32_t> version;
    std::vector<uint32_t> sourceId;
    std::vector<uint32_t> run;
    std::vector<uint32_t> lvl1Id;
    std::vector<uint32_t> bcid;
    std::vector<uint32_t> trigType;
    std::vector<uint32_t> lvl1DetType;
    std::vector<std::vector<uint32_t> > statusWords;
    std::vector<uint32_t> payloadSize;
    
  }; // class RODHeaderAuxContainer_v2 
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::RODHeaderAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_RODHEADERAUXCONTAINER_v2_H
