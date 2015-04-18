/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 604163 2014-06-30 10:30:46Z giagu $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v1.h"
#include "xAODTrigMuon/versions/L2IsoMuonContainer_v1.h"
#include "xAODTrigMuon/versions/L2CombinedMuonContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, L2StandAloneMuonContainer_v1 );
ADD_NS_DV_PROXY( xAOD, L2IsoMuonContainer_v1 );
ADD_NS_DV_PROXY( xAOD, L2CombinedMuonContainer_v1 );
