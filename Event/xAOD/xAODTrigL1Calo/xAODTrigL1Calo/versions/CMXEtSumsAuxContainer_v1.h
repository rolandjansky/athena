// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSumsAuxContainer_v1.h 687949 2015-08-06 15:48:49Z amazurov $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXETSUMSAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXETSUMSAUXCONTAINER_V1_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// Description of CMXEtSumsAuxContainer_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 687949 $
  /// $Date: 2015-08-06 17:48:49 +0200 (Thu, 06 Aug 2015) $

  class CMXEtSumsAuxContainer_v1 : public AuxContainerBase {
    public:
      /// Default constructor
      CMXEtSumsAuxContainer_v1();

    private:
      std::vector<uint8_t> crate;
      std::vector<uint8_t> sourceComponent;
      std::vector<uint8_t> peak;
      std::vector<std::vector<uint16_t> > etVec;
      std::vector<std::vector<uint16_t> > exVec;
      std::vector<std::vector<uint16_t> > eyVec;
      std::vector<std::vector<uint32_t> > etErrorVec;
      std::vector<std::vector<uint32_t> > exErrorVec;
      std::vector<std::vector<uint32_t> > eyErrorVec;

  }; // class CMXEtSumsAuxContainer_v1
} // namespace xAOD

SG_BASE( xAOD::CMXEtSumsAuxContainer_v1 , xAOD::AuxContainerBase );

#endif

