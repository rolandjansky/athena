/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 652111 2015-03-06 10:13:22Z krasznaa $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODLuminosity/versions/LumiBlockRangeContainer_v1.h"
#include "xAODLuminosity/versions/BCMRawDataContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, LumiBlockRangeContainer_v1 );
ADD_NS_DV_PROXY( xAOD, BCMRawDataContainer_v1 );
