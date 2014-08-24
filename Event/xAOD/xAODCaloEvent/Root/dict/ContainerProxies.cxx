/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 579810 2014-01-23 16:08:13Z krasznaa $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODCaloEvent/versions/CaloClusterContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, CaloClusterContainer_v1 );
