// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerAuxContainer_v2.h 687949 2015-08-06 15:48:49Z amazurov $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMTOWERAUXCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_CPMTOWERAUXCONTAINER_V2_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for CPMTower_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 687949 $
  /// $Date: 2015-08-06 17:48:49 +0200 (Thu, 06 Aug 2015) $    

  class CPMTowerAuxContainer_v2 : public AuxContainerBase {
  public:
    // Default constructor
    CPMTowerAuxContainer_v2();    

  private:
    std::vector<std::vector<uint8_t> > emEnergyVec;
    std::vector<std::vector<uint8_t> > hadEnergyVec;
    std::vector<std::vector<uint32_t> > emErrorVec;
    std::vector<std::vector<uint32_t> > hadErrorVec;
    std::vector<float> phi;
    std::vector<float> eta;
    std::vector<uint8_t> peak;  
    
  }; // class CPMTowerAuxContainer_v2
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CPMTowerAuxContainer_v2 , xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_CPMTOWERAUXCONTAINER_V2_H
