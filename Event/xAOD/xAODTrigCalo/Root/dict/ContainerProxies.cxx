/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 592214 2014-04-09 19:27:48Z krasznaa $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTrigCalo/versions/TrigCaloClusterContainer_v1.h"
#include "xAODTrigCalo/versions/TrigEMClusterContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TrigCaloClusterContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TrigEMClusterContainer_v1 );
