/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 602000 2014-06-16 16:35:14Z rticseto $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODBTagging/versions/BTaggingContainer_v1.h"
#include "xAODBTagging/versions/BTagVertexContainer_v1.h" 
// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, BTaggingContainer_v1 );
ADD_NS_DV_PROXY( xAOD, BTagVertexContainer_v1 ); 
