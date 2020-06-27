// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_VERSIONS_RINGSETCONFCONTAINER_V1_H
#define XAODCALORINGS_VERSIONS_RINGSETCONFCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODCaloRings/versions/RingSetConf_v1.h"

namespace xAOD {

/// The container is a simple typedef for now
typedef DataVector< xAOD::RingSetConf_v1 > RingSetConfContainer_v1;

} // namespace xAOD

#endif // XAODCALORINGS_VERSIONS_RINGSETCONFCONTAINER_V1_H
