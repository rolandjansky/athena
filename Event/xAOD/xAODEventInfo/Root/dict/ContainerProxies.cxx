/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 680052 2015-07-03 10:45:40Z krasznaa $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODEventInfo/versions/EventInfoContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, EventInfoContainer_v1 );
