/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTrigMinBias/versions/TrigSpacePointCountsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigT2MbtsBitsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigTrackCountsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigVertexCountsContainer_v1.h"
#include "xAODTrigMinBias/versions/TrigT2ZdcSignalsContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TrigSpacePointCountsContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TrigT2MbtsBitsContainer_v1 );                      
ADD_NS_DV_PROXY( xAOD, TrigTrackCountsContainer_v1 );                      
ADD_NS_DV_PROXY( xAOD, TrigVertexCountsContainer_v1 );                      
ADD_NS_DV_PROXY( xAOD, TrigT2ZdcSignalsContainer_v1 ); 
