// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMRoIAuxContainer_v1.h 631385 2014-11-26 21:27:40Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMROIAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_JEMROIAUXCONTAINER_V1_H

// STL include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for JEMRoI_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631385 $
  /// $Date: 2014-11-26 22:27:40 +0100 (Wed, 26 Nov 2014) $      

  class JEMRoIAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    JEMRoIAuxContainer_v1();

  private:
    std::vector<uint32_t> roiWord;
        
  }; // class JEMRoIAuxContainer_v1 
} // namespace xAOD

#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::JEMRoIAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_JEMROIAUXCONTAINER_V1_H
