// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoRawDataAuxContainer_v1.h 652824 2015-03-09 23:24:32Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_L1TOPORAWDATAAUXCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_L1TOPORAWDATAAUXCONTAINER_V2_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// AuxContainer for L1TopoRawData_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  /// @author Simon George <s.george@rhul.ac.uk>  
  ///
  /// $Revision: 652824 $
  /// $Date: 2015-03-10 00:24:32 +0100 (Tue, 10 Mar 2015) $

  class L1TopoRawDataAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    L1TopoRawDataAuxContainer_v1();

  private:
    std::vector<std::vector<uint32_t> > dataWords;
    std::vector<std::vector<uint32_t> > statusWords;
    std::vector<uint32_t> errorWord;
    std::vector<uint32_t> sourceID;
    
  }; // class L1TopoRawDataAuxContainer_v1  
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::L1TopoRawDataAuxContainer_v1 , xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_L1TOPORAWDATAAUXCONTAINER_V1_H
