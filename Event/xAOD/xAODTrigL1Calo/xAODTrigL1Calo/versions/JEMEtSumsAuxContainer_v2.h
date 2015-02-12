// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSumsAuxContainer_v2.h 646335 2015-02-12 01:16:10Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMETSUMSAUXCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_JEMETSUMSAUXCONTAINER_V2_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for JEMEtSums_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 646335 $
  /// $Date: 2015-02-12 02:16:10 +0100 (Thu, 12 Feb 2015) $    

  class JEMEtSumsAuxContainer_v2 : public AuxContainerBase {
  public:
    // Default constructor
    JEMEtSumsAuxContainer_v2();

  private:
    std::vector<uint_least8_t> crate;
    std::vector<uint_least8_t> module;
    std::vector<uint_least8_t> peak;
    std::vector<std::vector<uint_least16_t> > etVec;
    std::vector<std::vector<uint_least16_t> > exVec;
    std::vector<std::vector<uint_least16_t> > eyVec;
    
  }; // class JEMEtSumsAuxContainer_v2  
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::JEMEtSumsAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_JEMETSUMSAUXCONTAINER_V2_H
