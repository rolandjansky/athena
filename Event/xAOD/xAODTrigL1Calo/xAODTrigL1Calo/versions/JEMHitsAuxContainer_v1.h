// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMHitsAuxContainer_v1.h 576243 2013-12-19 11:24:27Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMHITSAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_JEMHITSAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for JEMHits_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 576243 $
  /// $Date: 2013-12-19 12:24:27 +0100 (Thu, 19 Dec 2013) $    

  class JEMHitsAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    JEMHitsAuxContainer_v1();

  private:
    std::vector<int> crate;
    std::vector<int> module;
    std::vector<int> peak;
    std::vector<std::vector<unsigned int> > jetHitsVec;
        
  }; // class JEMHitsAuxContainer_v1 
} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF( xAOD::JEMHitsAuxContainer_v1 , 1238207395 , 1 ) 
      
#include "SGTools/BaseInfo.h"
  SG_BASE( xAOD::JEMHitsAuxContainer_v1 , xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE
#endif // XAODTRIGL1CALO_VERSIONS_JEMHITSAUXCONTAINER_V1_H
