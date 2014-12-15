// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VertexContainer_v1.h 576511 2013-12-20 14:41:21Z salzburg $
#ifndef XAODTRACKING_VERSIONS_VERTEXCONTAINER_V1_H
#define XAODTRACKING_VERSIONS_VERTEXCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTracking/versions/Vertex_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::Vertex_v1 > VertexContainer_v1;
}

#endif // XAODTRACKING_VERSIONS_VERTEXCONTAINER_V1_H
