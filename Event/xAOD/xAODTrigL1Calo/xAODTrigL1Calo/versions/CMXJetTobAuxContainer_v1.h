// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetTobAuxContainer_v1.h 687949 2015-08-06 15:48:49Z amazurov $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXJETTOBAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXJETTOBAUXCONTAINER_V1_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// Description of CMXJetTobAuxContainer_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 687949 $
  /// $Date: 2015-08-06 17:48:49 +0200 (Thu, 06 Aug 2015) $

  class CMXJetTobAuxContainer_v1 : public AuxContainerBase {
    public:
      /// Default constructor
      CMXJetTobAuxContainer_v1();

    private:
      std::vector<uint8_t> crate;
      std::vector<uint8_t> jem;
      std::vector<uint8_t> frame;
      std::vector<uint8_t> location;
      std::vector<uint8_t> peak;
      std::vector<std::vector<uint16_t> > energyLargeVec;
      std::vector<std::vector<uint16_t> > energySmallVec;
      std::vector<std::vector<uint32_t> > errorVec;
      std::vector<std::vector<uint16_t> > presenceMapVec;

  }; // class CMXJetTobAuxContainer_v1
} // namespace xAOD

SG_BASE( xAOD::CMXJetTobAuxContainer_v1 , xAOD::AuxContainerBase );

#endif

