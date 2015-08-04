/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 670637 2015-05-28 15:49:43Z tbisanz $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTruth/versions/TruthParticleContainer_v1.h"
#include "xAODTruth/versions/TruthVertexContainer_v1.h"
#include "xAODTruth/versions/TruthEventContainer_v1.h"
#include "xAODTruth/versions/TruthPileupEventContainer_v1.h"
#include "xAODTruth/versions/TruthMetaDataContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TruthMetaDataContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TruthParticleContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TruthVertexContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TruthEventContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TruthPileupEventContainer_v1 );
