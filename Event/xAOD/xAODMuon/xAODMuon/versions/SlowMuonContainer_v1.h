/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_VERSIONS_SLOWMUONCONTAINER_V1_H
#define XAODMUON_VERSIONS_SLOWMUONCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODMuon/versions/SlowMuon_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::SlowMuon_v1 > SlowMuonContainer_v1;
}

#endif // XAODMUON_VERSIONS_SLOWMUONCONTAINER_V1_H
