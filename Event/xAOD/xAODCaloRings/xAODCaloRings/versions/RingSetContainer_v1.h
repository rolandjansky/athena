// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_VERSIONS_RINGSETCONTAINER_V1_H
#define XAODCALORINGS_VERSIONS_RINGSETCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODCaloRings/versions/RingSet_v1.h"

namespace xAOD {

/// The container is a simple typedef for now
typedef DataVector< xAOD::RingSet_v1 > RingSetContainer_v1;

} // namespace xAOD

#endif // XAODCALORINGS_VERSIONS_RINGSETCONTAINER_V1_H
