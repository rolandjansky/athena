/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _VrtSecInclusive_Tools_H
#define _VrtSecInclusive_Tools_H

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

namespace VKalVrtAthena {
  
  extern bool isAssociatedToVertices( const xAOD::TrackParticle *trk, const xAOD::VertexContainer* vertices );
  
  extern double vtxVtxDistance( const Amg::Vector3D& v1, const Amg::Vector3D& v2 );

}

#endif /* _VrtSecInclusive_Tools_H */
