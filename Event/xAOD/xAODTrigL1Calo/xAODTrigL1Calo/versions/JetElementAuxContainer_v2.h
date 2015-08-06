// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElementAuxContainer_v2.h 687949 2015-08-06 15:48:49Z amazurov $
#ifndef XAODTRIGL1CALO_VERSIONS_JETELEMENTAUXCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_JETELEMENTAUXCONTAINER_V2_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for JetElement_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 687949 $
  /// $Date: 2015-08-06 17:48:49 +0200 (Thu, 06 Aug 2015) $    

  class JetElementAuxContainer_v2 : public AuxContainerBase {
  public:
    // Default constructor
    JetElementAuxContainer_v2();

  private:
    std::vector<float> eta;
    std::vector<float> phi;    
    std::vector<unsigned int> key;
    std::vector<uint8_t>    peak;
    std::vector<std::vector<uint16_t> > emJetElementETVec;
    std::vector<std::vector<uint16_t> > hadJetElementETVec;
    std::vector<std::vector<uint32_t> > emJetElementErrorVec;
    std::vector<std::vector<uint32_t> > hadJetElementErrorVec;
    std::vector<std::vector<uint32_t> > linkErrorVec;
    
  }; // class JetElementAuxContainer_v2 
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::JetElementAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_JETELEMENTAUXCONTAINER_V2_H
