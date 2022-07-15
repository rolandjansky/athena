/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_VERSIONS_EFEXTOWERAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_EFEXTOWERAUXCONTAINER_V1_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// AuxContainer for eFexTower_v1

  class eFexTowerAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    eFexTowerAuxContainer_v1();
    
  private:   
    
    std::vector<float> eta;           
    std::vector<float> phi; 
    std::vector<uint32_t> module;
    std::vector<uint8_t> fpga;

    std::vector<std::vector<uint16_t> > et_count;

    std::vector<uint32_t> em_status;
    std::vector<uint32_t> had_status;
    
  }; // class eFexTowerAuxContainer_v1 
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::eFexTowerAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_EFEXTOWERAUXCONTAINER_V1_H
