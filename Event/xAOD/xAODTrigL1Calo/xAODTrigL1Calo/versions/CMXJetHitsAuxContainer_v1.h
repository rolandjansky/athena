// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHitsAuxContainer_v1.h 663426 2015-04-28 11:45:23Z amazurov $
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
  /// $Revision: 663426 $
  /// $Date: 2015-04-28 13:45:23 +0200 (Tue, 28 Apr 2015) $

  class CMXJetHitsAuxContainer_v1 : public AuxContainerBase {
    public:
      /// Default constructor
      CMXJetHitsAuxContainer_v1();

    private:
      std::vector<uint8_t> crate;
      std::vector<uint8_t> sourceComponent;
      std::vector<uint8_t> peak;
      std::vector<std::vector<uint8_t> > hitsVec0;
      std::vector<std::vector<uint8_t> > hitsVec1;
      std::vector<std::vector<uint8_t> > errorVec0;
      std::vector<std::vector<uint8_t> > errorVec1;

  }; // class CMXJetHitsAuxContainer_v2
} // namespace xAOD

SG_BASE( xAOD::CMXJetHitsAuxContainer_v1 , xAOD::AuxContainerBase );

#endif

