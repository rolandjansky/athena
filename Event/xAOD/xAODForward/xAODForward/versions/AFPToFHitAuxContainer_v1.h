/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARD_VERSIONS_AFPToFHITAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPToFHITAUXCONTAINER_V1_H

#include<vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

// local includes
#include "xAODForward/AFPToFHit.h"


namespace xAOD {

  /// @brief Class storing information of xAOD::AFPToFHit_v1 objects.
  ///
  /// This class actually stores information that is accessed by the xAOD::AFPToFHit_v1.
  class AFPToFHitAuxContainer_v1 : public AuxContainerBase {
 
  public:
    AFPToFHitAuxContainer_v1();
 
  private:
    std::vector< int > stationID;
    std::vector< float > time;
    std::vector< int > hptdcChannel;
    std::vector< int > barInTrainID;
    std::vector< int > trainID; 
    std::vector< float > pulseLength;
    std::vector< int > hptdcID;

  }; // class AFPToFHitAuxContainer_v1

  
} // end xAOD namespace

// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPToFHitAuxContainer_v1, xAOD::AuxContainerBase );


#endif // XAODFORWARD_VERSIONS_AFPToFHITAUXCONTAINER_V1_H

