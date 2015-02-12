// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSumsAuxContainer_v2.h 646335 2015-02-12 01:16:10Z morrisj $
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
  /// $Revision: 646335 $
  /// $Date: 2015-02-12 02:16:10 +0100 (Thu, 12 Feb 2015) $

  class CMXEtSumsAuxContainer_v2 : public AuxContainerBase {
    public:
      /// Default constructor
      CMXEtSumsAuxContainer_v2();

    private:
      std::vector<uint_least8_t> crate;
      std::vector<uint_least8_t> source;
      std::vector<uint_least8_t> peak;
      std::vector<std::vector<uint_least16_t> > etVec;
      std::vector<std::vector<uint_least16_t> > exVec;
      std::vector<std::vector<uint_least16_t> > eyVec;
      std::vector<std::vector<uint_least8_t> > etErrorVec;
      std::vector<std::vector<uint_least8_t> > exErrorVec;
      std::vector<std::vector<uint_least8_t> > eyErrorVec;

  }; // class CMXEtSumsAuxContainer_v2
} // namespace xAOD

SG_BASE( xAOD::CMXEtSumsAuxContainer_v2 , xAOD::AuxContainerBase );

#endif

