// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMEtSumsAuxContainer_v1.h 576243 2013-12-19 11:24:27Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMMETSUMSAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMMETSUMSAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for CMMEtSums_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 576243 $
  /// $Date: 2013-12-19 12:24:27 +0100 (Thu, 19 Dec 2013) $  

  class CMMEtSumsAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    CMMEtSumsAuxContainer_v1();

  private:
    std::vector<int> crate;
    std::vector<int> dataID;
    std::vector<int> peak;
    std::vector<std::vector<unsigned int> > etVec;
    std::vector<std::vector<unsigned int> > exVec;
    std::vector<std::vector<unsigned int> > eyVec;
    std::vector<std::vector<int> > etErrorVec;
    std::vector<std::vector<int> > exErrorVec;
    std::vector<std::vector<int> > eyErrorVec;
    
  }; // class CMMEtSumsAuxContainer_v1  
} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF( xAOD::CMMEtSumsAuxContainer_v1 , 1259127533 , 1 ) 
      
#include "SGTools/BaseInfo.h"
  SG_BASE( xAOD::CMMEtSumsAuxContainer_v1 , xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE
#endif // XAODTRIGL1CALO_VERSIONS_CMMETSUMSAUXCONTAINER_V1_H
