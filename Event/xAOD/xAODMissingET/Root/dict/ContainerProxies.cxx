/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 586551 2014-03-06 17:47:26Z krasznaa $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODMissingET/versions/MissingETContainer_v1.h"
#include "xAODMissingET/versions/MissingETComponentMap_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, MissingETContainer_v1 );
ADD_NS_DV_PROXY( xAOD, MissingETComponentMap_v1 );
