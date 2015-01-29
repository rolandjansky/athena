// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerAuxContainer_v1.h 631511 2014-11-27 13:17:07Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMTOWERAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CPMTOWERAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for CPMTower_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631511 $
  /// $Date: 2014-11-27 14:17:07 +0100 (Thu, 27 Nov 2014) $    

  class CPMTowerAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    CPMTowerAuxContainer_v1();

  private:
    std::vector<std::vector<int> > emEnergyVec;
    std::vector<std::vector<int> > hadEnergyVec;
    std::vector<std::vector<int> > emErrorVec;
    std::vector<std::vector<int> > hadErrorVec;
    std::vector<float> phi;
    std::vector<float> eta;
    std::vector<int> peak;  
    
  }; // class CPMTowerAuxContainer_v1
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CPMTowerAuxContainer_v1 , xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_CPMTOWERAUXCONTAINER_V1_H
