/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 670601 2015-05-28 14:22:49Z wsfreund $ 

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODCaloRings/versions/CaloRingsContainer_v1.h"
#include "xAODCaloRings/versions/RingSetContainer_v1.h"
#include "xAODCaloRings/versions/RingSetConfContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, RingSetContainer_v1       );
ADD_NS_DV_PROXY( xAOD, RingSetConfContainer_v1   );
ADD_NS_DV_PROXY( xAOD, CaloRingsContainer_v1     );

