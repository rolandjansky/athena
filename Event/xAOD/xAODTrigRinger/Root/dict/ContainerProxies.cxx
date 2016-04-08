/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx  $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"

#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v2.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v2.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TrigRingerRingsContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TrigRNNOutputContainer_v1 );

ADD_NS_DV_PROXY( xAOD, TrigRingerRingsContainer_v2 );
ADD_NS_DV_PROXY( xAOD, TrigRNNOutputContainer_v2 );



