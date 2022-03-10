/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _VrtSecFuzzy_Tools_H
#define _VrtSecFuzzy_Tools_H

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

namespace VKalVrtAthena {
  
  extern bool isAssociatedToVertices( const xAOD::TrackParticle *trk, const xAOD::VertexContainer* vertices );

}

#endif /* _VrtSecFuzzy_Tools_H */
