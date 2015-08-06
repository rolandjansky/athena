// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXRoIAuxContainer_v1.h 681007 2015-07-07 13:34:12Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXROIAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXROIAUXCONTAINER_V1_H

// STL include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"


namespace xAOD{
  
  /// AuxInfo for CMXRoI_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 681007 $
  /// $Date: 2015-07-07 15:34:12 +0200 (Tue, 07 Jul 2015) $    

  class CMXRoIAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    CMXRoIAuxContainer_v1();

  private:
    std::vector< std::vector<uint32_t> > cmxRoIWords;
    
  }; // class CMMRoIAuxInfo_v1  
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CMXRoIAuxContainer_v1 , xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_CMXROIAUXCONTAINER_V1_H
