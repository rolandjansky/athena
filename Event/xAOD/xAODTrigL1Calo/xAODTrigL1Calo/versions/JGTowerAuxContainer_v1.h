/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// Dear emacs, this is -*- c++ -*-
#ifndef XAODTRIGL1CALO_VERSIONS_JGTOWERAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_JGTOWERAUXCONTAINER_V1_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// AuxContainer for JGTower_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 652807 $
  /// $Date: 2015-03-09 22:52:07 +0100 (Mon, 09 Mar 2015) $
  ///
  class JGTowerAuxContainer_v1: public AuxContainerBase {
  public:
    // Default constructor
    JGTowerAuxContainer_v1();
    
  private:   
    
    // Tower identifiers
    std::vector<float> eta;           
    std::vector<float> phi; 
    std::vector<float> deta;
    std::vector<float> dphi;
    std::vector<float> et;
    std::vector<int>   Id;
    std::vector<int>   sampling;
    std::vector<std::vector<int>> SCIndex;
    std::vector<std::vector<int>> TileIndex;
  }; // class JGTowerAuxContainer 
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::JGTowerAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_JGTOWERAUXCONTAINER_V1_H
