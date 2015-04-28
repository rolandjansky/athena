// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSumsAuxContainer_v1.h 663426 2015-04-28 11:45:23Z amazurov $
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
  /// $Revision: 663426 $
  /// $Date: 2015-04-28 13:45:23 +0200 (Tue, 28 Apr 2015) $

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
      std::vector<std::vector<uint8_t> > etErrorVec;
      std::vector<std::vector<uint8_t> > exErrorVec;
      std::vector<std::vector<uint8_t> > eyErrorVec;

  }; // class CMXEtSumsAuxContainer_v1
} // namespace xAOD

SG_BASE( xAOD::CMXEtSumsAuxContainer_v1 , xAOD::AuxContainerBase );

#endif

