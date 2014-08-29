// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHEVENTCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHEVENTCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTruth/versions/TruthEvent_v1.h"

namespace xAOD {
   // Alias
   typedef DataVector< TruthEvent_v1 > TruthEventContainer_v1;
}

// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthEventContainer_v1 , 1200227006 , 1 )

#endif // XAODTRUTH_VERSIONS_TRUTHEVENTCONTAINER_V1_H
