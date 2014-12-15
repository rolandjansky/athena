/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERTEXCONTAINER_H
#define XAODTRACKING_VERTEXCONTAINER_H
 
// Local include(s):
#include "xAODTracking/Vertex.h"
#include "xAODTracking/versions/VertexContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current "Vertex container version"
   typedef VertexContainer_v1 VertexContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::VertexContainer, 1092961325, 1 )

#endif // XAODTRACKING_VERTEXCONTAINER_H
