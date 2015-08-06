// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHitsAuxContainer_v1.h 687949 2015-08-06 15:48:49Z amazurov $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXJETHITSAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXJETHITSAUXCONTAINER_V1_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// Description of CMXJetHitsAuxContainer_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 687949 $
  /// $Date: 2015-08-06 17:48:49 +0200 (Thu, 06 Aug 2015) $

  class CMXJetHitsAuxContainer_v1 : public AuxContainerBase {
    public:
      /// Default constructor
      CMXJetHitsAuxContainer_v1();

    private:
      std::vector<uint8_t> crate;
      std::vector<uint8_t> sourceComponent;
      std::vector<uint8_t> peak;
      std::vector<std::vector<uint32_t> > hitsVec0;
      std::vector<std::vector<uint32_t> > hitsVec1;
      std::vector<std::vector<uint32_t> > errorVec0;
      std::vector<std::vector<uint32_t> > errorVec1;

  }; // class CMXJetHitsAuxContainer_v2
} // namespace xAOD

SG_BASE( xAOD::CMXJetHitsAuxContainer_v1 , xAOD::AuxContainerBase );

#endif

