// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHitsAuxContainer_v2.h 642659 2015-01-29 12:41:06Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXJETHITSAUXCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_CMXJETHITSAUXCONTAINER_V2_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// Description of CMXJetHitsAuxContainer_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 642659 $
  /// $Date: 2015-01-29 13:41:06 +0100 (Thu, 29 Jan 2015) $

  class CMXJetHitsAuxContainer_v2 : public AuxContainerBase {
    public:
      /// Default constructor
      CMXJetHitsAuxContainer_v2();

    private:
      std::vector<int> crate;
      std::vector<int> source;
      std::vector<uint_least8_t> peak;
      std::vector<std::vector<unsigned int>> hitsVec0;
      std::vector<std::vector<unsigned int>> hitsVec1;
      std::vector<std::vector<int>> errorVec0;
      std::vector<std::vector<int>> errorVec1;

  }; // class CMXJetHitsAuxContainer_v2
} // namespace xAOD

SG_BASE( xAOD::CMXJetHitsAuxContainer_v2 , xAOD::AuxContainerBase );

#endif

