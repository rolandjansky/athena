/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 582152 2014-02-07 13:09:21Z krasznaa $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODPFlow/versions/PFOContainer_v1.h"
#include "xAODPFlow/versions/TrackCaloClusterContainer_v1.h"
#include "xAODPFlow/versions/FlowElementContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, PFOContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TrackCaloClusterContainer_v1 );
ADD_NS_DV_PROXY( xAOD, FlowElementContainer_v1 );
