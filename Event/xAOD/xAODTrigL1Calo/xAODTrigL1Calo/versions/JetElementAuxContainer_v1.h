// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElementAuxContainer_v1.h 631385 2014-11-26 21:27:40Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_JETELEMENTAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_JETELEMENTAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{
  
  /// AuxContainer for JetElement_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631385 $
  /// $Date: 2014-11-26 22:27:40 +0100 (Wed, 26 Nov 2014) $    

  class JetElementAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    JetElementAuxContainer_v1();

  private:
    std::vector<float> phi;
    std::vector<float> eta;
    std::vector<unsigned int> key;
    std::vector<int>    peak;
    std::vector<std::vector<int> > emEnergyVec;
    std::vector<std::vector<int> > hadEnergyVec;
    std::vector<std::vector<int> > emErrorVec;
    std::vector<std::vector<int> > hadErrorVec;
    std::vector<std::vector<int> > linkErrorVec;
    
  }; // class JetElementAuxContainer_v1 
} // namespace xAOD

#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::JetElementAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_JETELEMENTAUXCONTAINER_V1_H
