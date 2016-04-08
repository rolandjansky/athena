/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 698126 2015-10-02 12:59:53Z mishitsu $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v1.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v2.h"
#include "xAODTrigMuon/versions/L2IsoMuonContainer_v1.h"
#include "xAODTrigMuon/versions/L2CombinedMuonContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, L2StandAloneMuonContainer_v1 );
ADD_NS_DV_PROXY( xAOD, L2StandAloneMuonContainer_v2 );
ADD_NS_DV_PROXY( xAOD, L2IsoMuonContainer_v1 );
ADD_NS_DV_PROXY( xAOD, L2CombinedMuonContainer_v1 );
