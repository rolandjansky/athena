// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSumsAuxContainer_v2.h 642659 2015-01-29 12:41:06Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXETSUMSAUXCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_CMXETSUMSAUXCONTAINER_V2_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// Description of CMXEtSumsAuxContainer_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 642659 $
  /// $Date: 2015-01-29 13:41:06 +0100 (Thu, 29 Jan 2015) $

  class CMXEtSumsAuxContainer_v2 : public AuxContainerBase {
    public:
      /// Default constructor
      CMXEtSumsAuxContainer_v2();

    private:
      std::vector<int> crate;
      std::vector<int> source;
      std::vector<uint_least8_t> peak;
      std::vector<std::vector<unsigned int>> etVec;
      std::vector<std::vector<unsigned int>> exVec;
      std::vector<std::vector<unsigned int>> eyVec;
      std::vector<std::vector<int>> etErrorVec;
      std::vector<std::vector<int>> exErrorVec;
      std::vector<std::vector<int>> eyErrorVec;

  }; // class CMXEtSumsAuxContainer_v2
} // namespace xAOD

SG_BASE( xAOD::CMXEtSumsAuxContainer_v2 , xAOD::AuxContainerBase );

#endif

