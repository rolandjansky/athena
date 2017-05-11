/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHitsClusterContainer_v1.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-15
 * 
 * @brief  Header file with typedef for class AFPSiHitsClusterContainer_v1
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPSIHITSCLUSTERCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPSIHITSCLUSTERCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODForward/versions/AFPSiHitsCluster_v1.h"

namespace xAOD {
  typedef DataVector< AFPSiHitsCluster_v1 > AFPSiHitsClusterContainer_v1;
}
 

#endif //  XAODFORWARD_VERSIONS_AFPSIHITSCLUSTERCONTAINER_V1_H

