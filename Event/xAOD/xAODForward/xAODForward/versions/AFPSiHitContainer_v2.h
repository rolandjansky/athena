/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHitContainer_v2.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-26
 * 
 * @brief  Header file with typedef for class AFPSiHitContainer_v2
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPSIHITCONTAINER_V2_H
#define XAODFORWARD_VERSIONS_AFPSIHITCONTAINER_V2_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODForward/versions/AFPSiHit_v2.h"

namespace xAOD {
  typedef DataVector< AFPSiHit_v2 > AFPSiHitContainer_v2;
}
 

#endif //  XAODFORWARD_VERSIONS_AFPSIHITCONTAINER_V2_H

