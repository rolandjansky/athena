/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERTEXAUXCONTAINER_H
#define XAODTRACKING_VERTEXAUXCONTAINER_H
 
// Local include(s):
#include "xAODTracking/versions/VertexAuxContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current jet auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::TrackParticleContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef VertexAuxContainer_v1 VertexAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::VertexAuxContainer, 1320163892, 1 )

#endif // XAODTRACKING_VERTEXAUXCONTAINER_H
