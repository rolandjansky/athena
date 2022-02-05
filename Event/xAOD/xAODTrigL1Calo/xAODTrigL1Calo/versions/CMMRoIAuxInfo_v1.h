// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMRoIAuxInfo_v1.h 631511 2014-11-27 13:17:07Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_CMMROIAUXINFO_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMMROIAUXINFO_V1_H

// STL include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD{
  
  /// AuxInfo for CMMRoI_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631511 $
  /// $Date: 2014-11-27 14:17:07 +0100 (Thu, 27 Nov 2014) $    

  class CMMRoIAuxInfo_v1 : public AuxInfoBase {
  public:
    // Default constructor
    CMMRoIAuxInfo_v1();

  private:
    uint32_t jetEtRoiWord = 0U;
    uint32_t energyRoiWord0 = 0U;
    uint32_t energyRoiWord1 = 0U;
    uint32_t energyRoiWord2 = 0U;    
    
  }; // class CMMRoIAuxInfo_v1  
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CMMRoIAuxInfo_v1 , xAOD::AuxInfoBase );

#endif // XAODTRIGL1CALO_VERSIONS_CMMROIAUXINFO_V1_H
