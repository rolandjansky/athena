/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 592495 2014-04-11 08:47:29Z krasznaa $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTrigMissingET/versions/TrigMissingETContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TrigMissingETContainer_v1 );
