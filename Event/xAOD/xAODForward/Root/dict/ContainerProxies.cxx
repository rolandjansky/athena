/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 712044 2015-12-02 07:14:06Z nozka $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODForward/versions/ALFADataContainer_v1.h"
#include "xAODForward/versions/AFPDataContainer_v1.h"
#include "xAODForward/versions/ZdcModuleContainer_v1.h"
#include "xAODForward/versions/MBTSModuleContainer_v1.h"
#include "xAODForward/versions/ForwardEventInfoContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, ALFADataContainer_v1 );
ADD_NS_DV_PROXY( xAOD, AFPDataContainer_v1 );
ADD_NS_DV_PROXY( xAOD, ZdcModuleContainer_v1 );
ADD_NS_DV_PROXY( xAOD, MBTSModuleContainer_v1 );
ADD_NS_DV_PROXY( xAOD, ForwardEventInfoContainer_v1 );
