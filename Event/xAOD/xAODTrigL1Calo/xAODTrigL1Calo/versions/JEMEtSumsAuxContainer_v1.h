// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSumsAuxContainer_v1.h 576243 2013-12-19 11:24:27Z morrisj $
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
  /// $Revision: 576243 $
  /// $Date: 2013-12-19 12:24:27 +0100 (Thu, 19 Dec 2013) $    

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

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF( xAOD::JEMEtSumsAuxContainer_v1 , 1312707594 , 1 ) 
      
#include "SGTools/BaseInfo.h"
  SG_BASE( xAOD::JEMEtSumsAuxContainer_v1 , xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE
#endif // XAODTRIGL1CALO_VERSIONS_JEMETSUMSAUXCONTAINER_V1_H
