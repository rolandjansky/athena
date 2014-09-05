/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_PREPRAWDATACONTAINER_V1_H
#define XAODTRACKING_VERSIONS_PREPRAWDATACONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTracking/versions/PrepRawData_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::PrepRawData_v1 > PrepRawDataContainer_v1;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PrepRawDataContainer_v1 , 1082690359 , 1 )

#endif // XAODTRACKING_VERSIONS_PREPRAWDATACONTAINER_V1_H
