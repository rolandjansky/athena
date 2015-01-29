// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerAuxContainer_v2.h 642659 2015-01-29 12:41:06Z morrisj $
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
  /// $Revision: 642659 $
  /// $Date: 2015-01-29 13:41:06 +0100 (Thu, 29 Jan 2015) $    

  class CPMTowerAuxContainer_v2 : public AuxContainerBase {
  public:
    // Default constructor
    CPMTowerAuxContainer_v2();

  private:
    std::vector<std::vector<int> > emEnergyVec;
    std::vector<std::vector<int> > hadEnergyVec;
    std::vector<std::vector<int> > emErrorVec;
    std::vector<std::vector<int> > hadErrorVec;
    std::vector<float> phi;
    std::vector<float> eta;
    std::vector<uint_least8_t> peak;  
    
  }; // class CPMTowerAuxContainer_v2
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CPMTowerAuxContainer_v2 , xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_CPMTOWERAUXCONTAINER_V2_H
