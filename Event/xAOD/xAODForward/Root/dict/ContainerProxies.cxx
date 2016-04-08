/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 708124 2015-11-16 13:24:22Z steinber $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODForward/versions/ALFADataContainer_v1.h"
#include "xAODForward/versions/ZdcModuleContainer_v1.h"
#include "xAODForward/versions/MBTSModuleContainer_v1.h"
#include "xAODForward/versions/ForwardEventInfoContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, ALFADataContainer_v1 );
ADD_NS_DV_PROXY( xAOD, ZdcModuleContainer_v1 );
ADD_NS_DV_PROXY( xAOD, MBTSModuleContainer_v1 );
ADD_NS_DV_PROXY( xAOD, ForwardEventInfoContainer_v1 );
