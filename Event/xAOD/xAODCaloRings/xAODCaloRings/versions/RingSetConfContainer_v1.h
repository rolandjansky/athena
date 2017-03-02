/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetConfContainer_v1.h 707323 2015-11-12 02:45:01Z wsfreund $
#ifndef XAODCALORINGS_VERSIONS_RINGSETCONFCONTAINER_V1_H
#define XAODCALORINGS_VERSIONS_RINGSETCONFCONTAINER_V1_H

// STL includes(s):
#include <vector>

// Core include(s):
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODCaloRings/versions/RingSetConf_v1.h"

namespace xAOD {
/// The container is a simple typedef for now
typedef DataVector< xAOD::RingSetConf_v1 > RingSetConfContainer_v1;
/// Declare element links vector
typedef std::vector< ElementLink< RingSetConfContainer_v1 > >
  RingSetConfLinks_v1;
} // namespace xAOD

#endif // XAODCALORINGS_VERSIONS_RINGSETCONFCONTAINER_V1_H
