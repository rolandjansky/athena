// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMEtSumsAuxContainer_v1.h 631385 2014-11-26 21:27:40Z gwatts $
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
  /// $Revision: 631385 $
  /// $Date: 2014-11-26 22:27:40 +0100 (Wed, 26 Nov 2014) $  

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

#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::CMMEtSumsAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_CMMETSUMSAUXCONTAINER_V1_H
