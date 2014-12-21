/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODMuon/versions/MuonContainer_v1.h"
#include "xAODMuon/versions/MuonSegmentContainer_v1.h"
#include "xAODMuon/versions/SlowMuonContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, MuonContainer_v1 );
ADD_NS_DV_PROXY( xAOD, MuonSegmentContainer_v1 );
ADD_NS_DV_PROXY( xAOD, SlowMuonContainer_v1 );
