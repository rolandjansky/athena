// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSumsAuxContainer_v1.h 631385 2014-11-26 21:27:40Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMETSUMSAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_JEMETSUMSAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for JEMEtSums_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631385 $
  /// $Date: 2014-11-26 22:27:40 +0100 (Wed, 26 Nov 2014) $    

  class JEMEtSumsAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    JEMEtSumsAuxContainer_v1();

  private:
    std::vector<int> crate;
    std::vector<int> module;
    std::vector<int> peak;
    std::vector<std::vector<unsigned int> > etVec;
    std::vector<std::vector<unsigned int> > exVec;
    std::vector<std::vector<unsigned int> > eyVec;
    
  }; // class JEMEtSumsAuxContainer_v1  
} // namespace xAOD

#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::JEMEtSumsAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_JEMETSUMSAUXCONTAINER_V1_H
