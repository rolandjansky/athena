// -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHEVENTCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHEVENTCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTruth/versions/TruthEvent_v1.h"

namespace xAOD {
   /// Alias
   typedef DataVector< TruthEvent_v1 > TruthEventContainer_v1;
}

#endif // XAODTRUTH_VERSIONS_TRUTHEVENTCONTAINER_V1_H
