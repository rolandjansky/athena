// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerAuxContainer_v2.h 646335 2015-02-12 01:16:10Z morrisj $
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
  /// $Revision: 646335 $
  /// $Date: 2015-02-12 02:16:10 +0100 (Thu, 12 Feb 2015) $    

  class CPMTowerAuxContainer_v2 : public AuxContainerBase {
  public:
    // Default constructor
    CPMTowerAuxContainer_v2();    

  private:
    std::vector<std::vector<uint_least8_t> > emEnergyVec;
    std::vector<std::vector<uint_least8_t> > hadEnergyVec;
    std::vector<std::vector<uint_least8_t> > emErrorVec;
    std::vector<std::vector<uint_least8_t> > hadErrorVec;
    std::vector<float> phi;
    std::vector<float> eta;
    std::vector<uint_least8_t> peak;  
    
  }; // class CPMTowerAuxContainer_v2
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CPMTowerAuxContainer_v2 , xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_CPMTOWERAUXCONTAINER_V2_H
